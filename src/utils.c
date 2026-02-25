#include <stdio.h>
#include <stdint.h>
#include "utils.h"
#include "superblock.h"
#include "block.h"
#include "bitmap.h"

int init_fs(FILE* disk) {
    if (!init_superblock(disk)) return -1;
    if (!init_bitmap(disk)) return -1;
    if (!init_inode(disk)) return -1;
    return 0;
}

int mount_fs(FILE *disk, fs_t *fs) {
    superblock_t sbs = (superblock_t){0};

    if (!read_superblock(disk, &sbs)) return -1;
    if (sbs.magic != TF_FS_MAGIC) init_fs(disk);

    if (!read_superblock(disk, &(fs->sb))) return -1;
    if (!read_bitmap(disk, fs->bitmap)) return -1;
    if (!read_inode(disk, fs->table)) return -1;

    return 0;
}

int init_superblock(FILE *disk) {
    uint32_t free_blocks = TOTAL_BLOCKS - INODE_BLOCKS - BITMAP_BLOCKS - 1;
    superblock_t sb = (superblock_t){TF_FS_MAGIC, TOTAL_BLOCKS, free_blocks, INODE_BLOCK_START_BLOCK, BITMAP_BLOCK_START_BLOCK, DATA_BLOCK_START_BLOCK};

    fseek(disk, SUPERBLOCK_START, SEEK_SET);
    return fwrite(&sb, sizeof(superblock_t), 1, disk) == 1;
}

int init_bitmap(FILE *disk) {
    uint8_t bitmap[BITMAP_SIZE] = {0};

    bitmap[0] = 1u; // sb
    for (int i = BITMAP_BLOCK_START_BLOCK; i < BITMAP_BLOCK_START_BLOCK + BITMAP_BLOCKS; i++)
        bitmap[i/8] |= (1u << (i%8)); // bitmap
    for (int i = INODE_BLOCK_START_BLOCK; i < INODE_BLOCK_START_BLOCK + INODE_BLOCKS; i++)
        bitmap[i/8] |= (1u << (i%8)); // inode

             
    fseek(disk, BITMAP_BLOCK_START, SEEK_SET);
    return fwrite(bitmap, BITMAP_SIZE, 1, disk) == 1;
}

int init_inode(FILE *disk) {
    inode_t table[MAX_FILES] = {0};

    fseek(disk, INODE_BLOCK_START, SEEK_SET);
    return fwrite(table, sizeof(inode_t), MAX_FILES, disk) == MAX_FILES;
}

int read_superblock(FILE *disk, superblock_t *sb) {
    fseek(disk, SUPERBLOCK_START, SEEK_SET);
    return fread(sb, sizeof(superblock_t), 1, disk) == 1;
}

int read_bitmap(FILE *disk, uint8_t bitmap[]) {
    fseek(disk, BITMAP_BLOCK_START, SEEK_SET);
    return fread(bitmap, BITMAP_SIZE, 1, disk) == 1;
}

int read_inode(FILE *disk, inode_t table[]) {
    fseek(disk, INODE_BLOCK_START, SEEK_SET);
    return fread(table, sizeof(inode_t), MAX_FILES, disk) == MAX_FILES;
}

int sync_fs(FILE *disk, superblock_t *sb, uint8_t bitmap[], inode_t table[]) {
    fseek(disk, SUPERBLOCK_START, SEEK_SET);
    if (fwrite(sb, sizeof(superblock_t), 1, disk) != 1) return -1;
    fseek(disk, BITMAP_BLOCK_START, SEEK_SET);
    if (fwrite(bitmap, BITMAP_SIZE, 1, disk) != 1) return -1;
    fseek(disk, INODE_BLOCK_START, SEEK_SET);
    if (fwrite(table, sizeof(inode_t), MAX_FILES, disk) != MAX_FILES) return -1;

    fflush(disk);
    return 0;
}
