#pragma once

#include <stdint.h>

#define MAX_BLOCKS_PER_FILE 16
#define MAX_FILE_NAME 128

typedef struct inode_t {
    uint32_t blocks[MAX_BLOCKS_PER_FILE];
    char name[MAX_FILE_NAME];
    uint32_t size;
    uint8_t used;
} inode_t;

/* Finds and returns pointer to inode in inode table based off name. */
inode_t *find_inode(inode_t table[], const char *name);

/* Creates and returns pointer to inode in inode table. */
inode_t *create_inode(inode_t table[], const char *name);

/* Frees inode so it can be used again. */
int free_inode(inode_t table[], const char *name);
