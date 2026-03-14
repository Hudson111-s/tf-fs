#define FUSE_USE_VERSION 28

#include <fuse.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <time.h>

#include "tf-fs.h"
#include "inode.h"
#include "disk.h"
#include "utils.h"

// Global fs instance.
static fs_t fs;

static int fs_getattr(const char *path, struct fuse_stat *st) {
    memset(st, 0, sizeof(*st));

    // Seamless Permissions, current user is owner.
    st->st_uid = fuse_get_context()->uid;
    st->st_gid = fuse_get_context()->gid;

    if (strcmp(path, "/") == 0) {
        st->st_mode = S_IFDIR | 0755;
        st->st_nlink = 2;
        return 0;
    }

    inode_t *inode = find_inode(fs.table, path + 1);
    if (!inode) return -ENOENT;
    
    st->st_atim.tv_sec = inode->atime;
    st->st_atim.tv_nsec = 0;
    st->st_mtim.tv_sec = inode->mtime;
    st->st_mtim.tv_nsec = 0;
    st->st_ctim.tv_sec = inode->mtime;
    st->st_ctim.tv_nsec = 0;

    st->st_mode = S_IFREG | 0666;
    st->st_nlink = 1;
    st->st_size = inode->size;
    return 0;
}

static int fs_readdir(
    const char *path,
    void *buf,
    fuse_fill_dir_t filler,
    fuse_off_t offset,
    struct fuse_file_info *fi)
{
    (void)offset;
    (void)fi;

    // Only check root.
    if (strcmp(path, "/") != 0) return -ENOENT;

    filler(buf, ".", NULL, 0);
    filler(buf, "..", NULL, 0);

    for (int i = 0; i < MAX_FILES; i++) {
        if (fs.table[i].used) filler(buf, fs.table[i].name, NULL, 0);
    }

    return 0;
}

static int fs_truncate(const char *path, fuse_off_t size) {
    inode_t *inode = find_inode(fs.table, path + 1);
    if (!inode) return -ENOENT;

    return truncate_file(&fs, inode, size);
}

static int fs_create(const char *path, fuse_mode_t mode, struct fuse_file_info *fi) {
    (void)mode; 
    (void)fi;

    const char *name = path + 1;
    inode_t *inode = find_inode(fs.table, name);

    // If file already exists, truncate.
    if (inode) return truncate_file(&fs, inode, 0);

    inode = create_file(&fs, name);
    if (!inode) return -ENOSPC;

    time_t now = time(NULL);
    inode->atime = (int64_t)now;
    inode->mtime = (int64_t)now;

    sync_fs(&fs);
    return 0;
}

static int fs_rename(const char *from, const char *to) {
    inode_t *inode = find_inode(fs.table, from + 1);
    if (!inode) return -ENOENT;

    strncpy(inode->name, to + 1, MAX_FILE_NAME);
    sync_fs(&fs);
    return 0;
}

static int fs_read(
    const char *path,
    char *buf,
    size_t size,
    fuse_off_t offset,
    struct fuse_file_info *fi)
{
    (void)fi;

    inode_t *inode = find_inode(fs.table, path + 1);
    if (!inode) return -ENOENT;

    return read_file(&fs, inode, (uint8_t *)buf, size, offset);
}

static int fs_write(
    const char *path,
    const char *buf,
    size_t size,
    fuse_off_t offset,
    struct fuse_file_info *fi)
{
    (void)fi;

    inode_t *inode = find_inode(fs.table, path + 1);
    if (!inode) return -ENOENT;
    
    int written = write_file(&fs, inode, (uint8_t *)buf, size, offset);
    return written == -1 ? -EFBIG : written;
}

static int fs_open(const char *path, struct fuse_file_info *fi) {
    (void)fi;

    inode_t *inode = find_inode(fs.table, path + 1);
    if (!inode) return -ENOENT;

    return 0; 
}

static int fs_unlink(const char *path)
{
    inode_t *inode = find_inode(fs.table, path + 1);
    if (!inode) return -ENOENT;

    return delete_file(&fs, inode);
}

static int fs_utimens(const char *path, const struct fuse_timespec tv[2])
{
    inode_t *inode = find_inode(fs.table, path + 1);
    if (!inode) return -ENOENT;

    inode->atime = tv[0].tv_sec;
    inode->mtime = tv[1].tv_sec;

    sync_fs(&fs);
    return 0;
}

static int fs_statfs(const char *path, struct fuse_statvfs *st) {
    (void)path;
    
    st->f_bsize = BLOCK_SIZE;
    st->f_frsize = BLOCK_SIZE;
    st->f_blocks = TOTAL_BLOCKS;
    st->f_bfree = fs.sb.free_blocks;
    st->f_bavail = fs.sb.free_blocks;
    
    st->f_files = MAX_FILES;
    st->f_namemax = MAX_FILE_NAME;
    
    int inodes_free = 0;
    for (int i = 0; i < MAX_FILES; i++) {
        if (!fs.table[i].used) inodes_free++;
    }
    st->f_ffree = inodes_free; 

    return 0;
}

int fs_flush(const char *path, struct fuse_file_info *fi) {
    (void)path;
    (void)fi;

    sync_fs(&fs);
    return 0;
}

static struct fuse_operations ops = {
    .getattr  = fs_getattr,
    .readdir  = fs_readdir,
    .create   = fs_create,
    .open     = fs_open,
    .read     = fs_read,
    .write    = fs_write,
    .unlink   = fs_unlink,
    .utimens  = fs_utimens,
    .truncate = fs_truncate,
    .statfs   = fs_statfs,
    .rename   = fs_rename,
    .flush    = fs_flush
};

int main(int argc, char *argv[])
{
    if (argc < 3) {
        printf("usage: %s <disk.img> <mountpoint>\n", argv[0]);
        return 1;
    }

    FILE *disk = open_disk(argv[1]);
    if (!disk) {
        perror("open_disk");
        return 1;
    }

    if (mount_fs(disk, &fs) != 0) {
        printf("Failed to mount filesystem\n");
        return 1;
    }

    return fuse_main(argc - 1, argv + 1, &ops, NULL);
}