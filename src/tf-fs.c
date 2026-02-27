#include <stdio.h>
#include <string.h>
#include "tf-fs.h"
#include "superblock.h"
#include "bitmap.h"
#include "inode.h"
#include "block.h"
#include "utils.h"

inode_t *create_file(fs_t *fs, const char *name) {
    inode_t *inode = create_inode(fs->table, name);
    sync_fs(fs);
    return inode;
}

int write_file(fs_t *fs, inode_t *inode, uint8_t data[], size_t size) {
    int block_index = inode->size / BLOCK_SIZE;
    size_t used = inode->size % BLOCK_SIZE;

    size_t remaining = size;
    uint8_t *src = data;

    while (remaining > 0) {
        if (block_index >= MAX_BLOCKS_PER_FILE)
            return -3;

        int new_block = 0;
        if (inode->blocks[block_index] == 0) {
            int b = alloc_block(fs->bitmap, &fs->sb);
            if (b == -1) return -2;

            inode->blocks[block_index] = b;
            new_block = 1;
        }

        int block = inode->blocks[block_index];
        uint8_t buffer[BLOCK_SIZE] = {0};
        if (!new_block) read_block(fs->disk, block, buffer);

        size_t free = BLOCK_SIZE - used;
        size_t to_write = remaining < free ? remaining : free;

        memcpy(buffer + used, src, to_write);
        write_block(fs->disk, block, buffer);

        inode->size += to_write;
        remaining -= to_write;
        src += to_write;

        block_index++;
        used = 0;
    }

    return sync_fs(fs);
}

int read_file(fs_t *fs, inode_t *inode, uint8_t out[], size_t size) {
    if (inode->size > size) return -2;

    int total_blocks = (inode->size + BLOCK_SIZE - 1) / BLOCK_SIZE;

    for (int i = 0; i < total_blocks; i++) {
        if (inode->blocks[i] == 0) return -1;
        
        size_t remaining = inode->size - i * BLOCK_SIZE;
        size_t to_read = remaining < BLOCK_SIZE ? remaining : BLOCK_SIZE;

        if (!read_bytes(fs->disk, inode->blocks[i], to_read, out + i * BLOCK_SIZE)) return -1;
    }

    return inode->size; // Amount of bytes read if not failed
}
