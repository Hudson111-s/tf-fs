#include <stdio.h>
#include <stdint.h>
#include "block.h"
#include "superblock.h"
#include "bitmap.h"
#include "inode.h"

FILE *open_disk(const char *path) {
    return fopen(path, "r+b");
}

int read_block(FILE *disk, int block_num, uint8_t *buffer) {
    if (fseek(disk, block_num * BLOCK_SIZE, SEEK_SET) != 0) return -1;
    return fread(buffer, BLOCK_SIZE, 1, disk) == 1;
}

int write_block(FILE *disk, int block_num, uint8_t *buffer) {
    if (fseek(disk, block_num * BLOCK_SIZE, SEEK_SET) != 0) return -1;
    return fwrite(buffer, BLOCK_SIZE, 1, disk) == 1;
}

int read_bytes(FILE *disk, int block_num, size_t size, uint8_t *buffer) {
    if (size > BLOCK_SIZE) return -1;

    if (fseek(disk, block_num * BLOCK_SIZE, SEEK_SET) != 0) return -1;
    return fread(buffer, size, 1, disk) == 1;
}

int write_bytes(FILE *disk, int block_num, size_t size, uint8_t *buffer) {
    if (size > BLOCK_SIZE) return -1;

    if (fseek(disk, block_num * BLOCK_SIZE, SEEK_SET) != 0) return -1;
    return fwrite(buffer, size, 1, disk) == 1;
}
