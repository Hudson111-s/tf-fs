#pragma once

#include <stdint.h>

#define TF_FS_MAGIC 0x54464653

typedef struct superblock_t{
    uint32_t magic;
    uint32_t total_blocks;
    uint32_t free_blocks;
    uint32_t inode_start;
    uint32_t bitmap_start;
    uint32_t data_start;
} superblock_t;
