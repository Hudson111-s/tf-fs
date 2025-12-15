#include <stdio.h>
#include <stdint.h>
#include "include/utils.h"
#include "include/superblock.h"
#include "include/block.h"
#include "include/bitmap.h"

int init_fs(FILE* disk) {
    if (!init_superblock(disk)) return -1;
    if (!init_bitmap(disk)) return -1;
    if (!init_inode(disk)) return -1;
    return 0;
}

int init_superblock(FILE *disk) {
    uint32_t free_blocks = TOTAL_BLOCKS - INODE_BLOCKS - BITMAP_BLOCKS - 1;
    superblock_t *sb = (superblock_t *){TF_FS_MAGIC, TOTAL_BLOCKS, free_blocks, INODE_BLOCK_START, DATA_BLOCK_START};

    fseek(disk, SUPERBLOCK_START, SEEK_SET);
    return fwrite(sb, sizeof(superblock_t), 1, disk) == 1;
}

int init_bitmap(FILE *disk) {
    uint8_t bitmap[BITMAP_SIZE] = {0};

    fseek(disk, BITMAP_BLOCK_START, SEEK_SET);
    return fwrite(bitmap, BITMAP_SIZE, 1, disk) == 1;
}

int init_inode(FILE *disk) {
    inode_t table[MAX_FILES] = {0};

    fseek(disk, INODE_BLOCK_START, SEEK_SET);
    return fwrite(table, sizeof(inode_t), MAX_FILES, disk) == MAX_FILES;
}
