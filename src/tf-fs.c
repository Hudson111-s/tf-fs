#include <stdio.h>
#include <string.h>
#include "../include/superblock.h"
#include "../include/bitmap.h"
#include "../include/inode.h"
#include "../include/block.h"
#include "../include/utils.h"

inode_t *create_file(inode_t table[], const char *name) {
    return create_inode(table, name);
}

int write_file(FILE *disk, superblock_t *sb, int8_t bitmap[], inode_t table[], inode_t *inode, uint8_t data[], size_t size) {
    int blocks_needed = (size + BLOCK_SIZE - 1) / BLOCK_SIZE;
    for (int i = 0; i < blocks_needed; i++) {
        int block = alloc_block(bitmap, sb);
        if (block == -1) return -1;
        inode->blocks[i] = block;

        size_t remaining = size - i * BLOCK_SIZE;
        size_t to_write = remaining < BLOCK_SIZE ? remaining : BLOCK_SIZE;

        if (!write_bytes(disk, block, to_write, data + i * BLOCK_SIZE)) return -1;
    }

    inode->size = size;
    sync_fs(disk, sb, bitmap, table);

    return 0;
}

int read_file(FILE *disk, inode_t *inode, uint8_t out[]) {
    int total_blocks = (inode->size + BLOCK_SIZE - 1) / BLOCK_SIZE;

    for (int i = 0; i < total_blocks; i++) {
        if (inode->blocks[i] == 0) break;
        
        size_t remaining = inode->size - i * BLOCK_SIZE;
        size_t to_read = remaining < BLOCK_SIZE ? remaining : BLOCK_SIZE;

        if (!read_bytes(disk, inode->blocks[i], to_read, out + i * BLOCK_SIZE)) return -1;
    }

    return inode->size; // Amount of bytes read if not failed
}

int read_file_bname(FILE *disk, inode_t table[], uint8_t out[], const char *name) {
   inode_t *inode = find_inode(table, name);
   if (inode == NULL) return -1;
   return read_file(disk, inode, out);
}
