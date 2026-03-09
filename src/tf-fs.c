#include <stdio.h>
#include <string.h>
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

int write_file(fs_t *fs, inode_t *inode, uint8_t data[], size_t size, size_t offset) {
    int block_index = offset / BLOCK_SIZE;
    size_t block_offset = offset % BLOCK_SIZE;

    size_t remaining = size;
    size_t written = 0;
    uint8_t *src = data;

    while (remaining > 0) {
        if (block_index >= MAX_BLOCKS_PER_FILE) return -3;
        
        int block = inode->blocks[block_index];
        int new_block = 0;
        if (block == 0) {
            block = alloc_block(fs->bitmap, &fs->sb);
            if (block == -1) return -2;

            inode->blocks[block_index] = block;
            new_block = 1;
        }

        uint8_t buffer[BLOCK_SIZE] = {0};
        if (!new_block && (block_offset != 0 || remaining < BLOCK_SIZE)) {
            read_block(fs->disk, block, buffer);
        }

        size_t space = BLOCK_SIZE - block_offset;
        size_t to_write = remaining < space ? remaining : space;

        memcpy(buffer + block_offset, src + written, to_write);
        write_block(fs->disk, block, buffer);

        written += to_write;
        remaining -= to_write;

        block_index++;
        block_offset = 0;
    }
    size_t new_end = offset + written;
    if (new_end > inode->size) inode->size = new_end;

    return sync_fs(fs);
}

int read_file(fs_t *fs, inode_t *inode, uint8_t out[], size_t size, size_t offset) {
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

        read_bytes(fs->disk, block, out + total_read, to_read, block_offset);

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
        
        uint8_t buffer[BLOCK_SIZE] = {0};
        if (write_block(fs->disk, inode->blocks[i], buffer) == -1) return -1;
        free_block(fs->bitmap, &fs->sb, inode->blocks[i]);
    }
    if (free_inode(fs->table, inode->name) == -1) return -1;

    return sync_fs(fs);
}
