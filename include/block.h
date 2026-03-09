#pragma once

#include <stdio.h>
#include "disk.h"
#include "inode.h"
#include "superblock.h"

#define BLOCK_SIZE 4096
#define TOTAL_BLOCKS ((DISK_MB * 1024 * 1024) / BLOCK_SIZE)

#define BITMAP_BITS_PER_BLOCK (BLOCK_SIZE * 8)
#define BITMAP_BLOCKS ((TOTAL_BLOCKS + BITMAP_BITS_PER_BLOCK - 1) / BITMAP_BITS_PER_BLOCK)

#define INODE_BLOCKS (TOTAL_BLOCKS / 50)  // 2% for inodes

/* In bytes. */
#define SUPERBLOCK_START 0 
#define BITMAP_BLOCK_START (SUPERBLOCK_START + BLOCK_SIZE)
#define INODE_BLOCK_START (BITMAP_BLOCK_START + BITMAP_BLOCKS * BLOCK_SIZE)
#define DATA_BLOCK_START (INODE_BLOCK_START + INODE_BLOCKS * BLOCK_SIZE)

/* Block index. */
#define SUPERBLOCK_START_BLOCK (SUPERBLOCK_START / BLOCK_SIZE)
#define BITMAP_BLOCK_START_BLOCK (BITMAP_BLOCK_START / BLOCK_SIZE)
#define INODE_BLOCK_START_BLOCK (INODE_BLOCK_START / BLOCK_SIZE)
#define DATA_BLOCK_START_BLOCK (DATA_BLOCK_START / BLOCK_SIZE)

#define MAX_FILES (INODE_BLOCKS * (BLOCK_SIZE / sizeof(inode_t)))

/* Opens disk and returns pointer to it. */
FILE *open_disk(const char *path);

/* Reads block based on block_num and stores it in buffer. */
int read_block(FILE *disk, int block_num, uint8_t *buffer);

/* Writes to block based on block_num. */
int write_block(FILE *disk, int block_num, uint8_t *buffer);

/* Reads bytes from block, up to one block */
int read_bytes(FILE *disk, int block_num, uint8_t *buffer, size_t size, size_t offset);

/* Write bytes to block, up to one block */
int write_bytes(FILE *disk, int block_num, uint8_t *buffer, size_t size, size_t offset);
