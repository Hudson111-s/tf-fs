#pragma once

#include <stdint.h>
#include "block.h"
#include "superblock.h"

#define BITMAP_SIZE (BITMAP_BLOCKS * BLOCK_SIZE)

/* Allocates block in bitmap. */
int alloc_block(uint8_t bitmap[], superblock_t *sb);

/* Frees block bit in bitmap. */
void free_block(uint8_t bitmap[], superblock_t *sb, int block);
