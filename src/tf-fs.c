#include <stdio.h>
#include <string.h>
#include <errno.h>

#include "tf-fs.h"
#include "superblock.h"
#include "bitmap.h"
#include "inode.h"
#include "block.h"
#include "utils.h"

inode_t *create_file(fs_t *fs, const char *name) {
    if (find_inode(fs->table, name) != NULL) return NULL;

    inode_t *inode = create_inode(fs->table, name);
    sync_fs(fs);
    return inode;
}

size_t write_file(fs_t *fs, inode_t *inode, uint8_t data[], size_t size, size_t offset) {
    int block_index = offset / BLOCK_SIZE;
    size_t block_offset = offset % BLOCK_SIZE;

    size_t remaining = size;
    size_t written = 0;

    while (remaining > 0) {
        if (block_index >= MAX_BLOCKS_PER_FILE) {
            if (written > 0) break;
            return -EFBIG;
        }
        
        int block = inode->blocks[block_index];
        int new_block = 0;
        if (block == 0) {
            block = alloc_block(fs->bitmap, &fs->sb);
            if (block == -1) {
                if (written > 0) break;
                return -ENOSPC; // Disk is full
            }

            inode->blocks[block_index] = block;
            new_block = 1;
        }

        uint8_t buffer[BLOCK_SIZE] = {0};
        if (!new_block && (block_offset != 0 || remaining < BLOCK_SIZE)) {
            if (read_block(fs->disk, block, buffer) == -1) return -EIO;
        }

        size_t space = BLOCK_SIZE - block_offset;
        size_t to_write = remaining < space ? remaining : space;

        if (data != NULL) {
            memcpy(buffer + block_offset, data + written, to_write);
        } else {
            memset(buffer + block_offset, 0, to_write);
        }
        
        // Allows partial writes.
        if (write_block(fs->disk, block, buffer) == -1) {
            if (written > 0) break;
            return -EIO;
        }

        written += to_write;
        remaining -= to_write;

        block_index++;
        block_offset = 0;
    }
    size_t new_end = offset + written;
    if (new_end > inode->size) inode->size = new_end;
    
    return written;
}

size_t read_file(fs_t *fs, inode_t *inode, uint8_t out[], size_t size, size_t offset) {
    if (offset >= inode->size) return 0;
    if (offset + size > inode->size) size = inode->size - offset;

    int block_index = offset / BLOCK_SIZE;
    size_t block_offset = offset % BLOCK_SIZE;
    size_t total_read = 0;

    while (total_read < size) {
        if (block_index >= MAX_BLOCKS_PER_FILE) break;

        int block = inode->blocks[block_index];
        if (block == 0) break;

        size_t space = BLOCK_SIZE - block_offset;
        size_t remaining = size - total_read;
        size_t to_read = remaining < space ? remaining : space;

        if (read_bytes(fs->disk, block, out + total_read, to_read, block_offset) == -1)
            return -EIO;

        total_read += to_read;
        block_index++;
        block_offset = 0;
    }

    return total_read;
}

int delete_file(fs_t *fs, inode_t *inode) {
    int total_blocks = (inode->size + BLOCK_SIZE - 1) / BLOCK_SIZE;
    for (int i = 0; i < total_blocks; i++) {
        if (inode->blocks[i] == 0) continue;
        free_block(fs->bitmap, &fs->sb, inode->blocks[i]);
    }
    free_inode(inode);

    if (sync_fs(fs) == -1) return -EIO;
    return 0;
}

int truncate_file(fs_t *fs, inode_t *inode, size_t size) {
    size_t old_size = inode->size;

    // File gets smaller.
    if (size < old_size) {
        int old_blocks = (old_size + BLOCK_SIZE - 1) / BLOCK_SIZE;
        int new_blocks = (size + BLOCK_SIZE - 1) / BLOCK_SIZE;

        for (int i = new_blocks; i < old_blocks; i++) {
            if (inode->blocks[i]) {
                free_block(fs->bitmap, &fs->sb, inode->blocks[i]);
                inode->blocks[i] = 0;
            }
        }
        inode->size = size;
    }

    // File gets bigger.
    if (size > old_size) {
        size_t written = write_file(fs, inode, NULL, size - old_size, old_size);
        if (written < 0) return (int)written;
    }

    sync_fs(fs);
    return 0;
}
