#pragma once

#include <stdio.h>
#include <stdint.h>
#include "superblock.h"
#include "bitmap.h"
#include "inode.h"

typedef struct fs_t{
    FILE *disk;
    superblock_t sb;
    uint8_t bitmap[BITMAP_SIZE];
    inode_t table[MAX_FILES];
} fs_t;

/* Creates and saves file. Does not allocate block(s). */
inode_t *create_file(fs_t *fs, const char *name);

/* Appends data buffer to file. Will allocate block(s) if needed. */
int write_file(fs_t *fs, inode_t *inode, uint8_t data[], size_t size);

/* Reads file into out buffer. */
int read_file(fs_t *fs, inode_t *inode, uint8_t out[], size_t size);
