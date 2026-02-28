#include <stdint.h>
#include "block.h"
#include "superblock.h"

int alloc_block(uint8_t bitmap[], superblock_t *sb) {
    for (uint32_t b = DATA_BLOCK_START_BLOCK; b < sb->total_blocks; b++) {
        if(!(bitmap[b/8] & (1u << (b%8)))) {
            bitmap[b/8] |= (1u << (b%8));
            sb->free_blocks--;
            return b;
        }
    }
    return -1;
}

void free_block(uint8_t bitmap[], superblock_t *sb, int block) {
    bitmap[block/8] &= ~(1u << (block%8));
    sb->free_blocks++;
}
