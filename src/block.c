#include <stdio.h>
#include <stdint.h>
#include "include/block.h"
#include "include/superblock.h"
#include "include/bitmap.h"
#include "include/inode.h"

FILE *open_disk(const char *path) {
    return fopen(path, "r+b");
}

int sync_metadata(FILE *disk, superblock_t *sb, uint8_t bitmap[], inode_t table[]) {
    fseek(disk, SUPERBLOCK_START, SEEK_SET);
    if (fwrite(sb, sizeof(superblock_t), 1, disk) != 1) return -1;
    fseek(disk, BITMAP_BLOCK_START, SEEK_SET);
    if (fwrite(bitmap, BITMAP_SIZE, 1, disk) != 1) return -1;
    fseek(disk, INODE_BLOCK_START, SEEK_SET);
    if (fwrite(table, sizeof(inode_t), MAX_FILES, disk) != MAX_FILES) return -1;

    fflush(disk);
    return 0;
}

int read_block(FILE *disk, int block_num, void *buffer) {
    fseek(disk, block_num * BLOCK_SIZE, SEEK_SET);
    return fread(buffer, BLOCK_SIZE, 1, disk) == 1;
}

int write_block(FILE *disk, int block_num, void *buffer) {
    fseek(disk, block_num * BLOCK_SIZE, SEEK_SET);
    return fwrite(buffer, BLOCK_SIZE, 1, disk) == 1;
}
