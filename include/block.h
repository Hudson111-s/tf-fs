#pragma once

#include <stdio.h>
#include "disk.h"
#include "inode.h"
#include "superblock.h"

#define BLOCK_SIZE 4096
#define TOTAL_BLOCKS (DISK_MB * 256) // 256 is the ratio of 4KB blocks to MB

#define BITMAP_BLOCKS 1 // Total blocks taken
#define INODE_BLOCKS 8 // Total blocks taken

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
int read_bytes(FILE *disk, int block_num, size_t size, uint8_t *buffer);

/* Write bytes to block, up to one block */
int write_bytes(FILE *disk, int block_num, size_t size, uint8_t *buffer);
