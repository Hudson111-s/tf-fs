#pragma once

#include <stdio.h>
#include "disk/disk.h"
#include "inode.h"
#include <superblock.h>

#define BLOCK_SIZE 4096
#define TOTAL_BLOCKS (DISK_MB * 256) // 256 is the ratio of 4KB blocks to MB
#define BITMAP_BLOCKS 1 // Total blocks
#define INODE_BLOCKS 8 // Total blocks
#define SUPERBLOCK_START 0 // In bytes
#define BITMAP_BLOCK_START (SUPERBLOCK_START + BLOCK_SIZE) // In bytes
#define INODE_BLOCK_START (BITMAP_BLOCK_START + BITMAP_BLOCKS * BLOCK_SIZE) // In bytes
#define DATA_BLOCK_START (INODE_BLOCK_START + INODE_BLOCKS * BLOCK_SIZE) // In bytes
#define MAX_FILES (INODE_BLOCKS * (BLOCK_SIZE / sizeof(inode_t)))

/* Opens disk and returns pointer to it. */
FILE *open_disk(const char *path);

/* Writes sb, bitmap, and all inodes to disk. */
int sync_metadata(FILE *disk, superblock_t *sb, uint8_t bitmap[], inode_t table[]);

/* Reads block based on block_num and stores it in buffer. */
int read_block(FILE *disk, int block_num, void *buffer);

/* Writes to block based on block_num. */
int write_block(FILE *disk, int block_num, void *buffer);
