#pragma once

#include <stdint.h>

typedef struct inode_t {
    char name[64];         
    uint32_t size;
    uint32_t blocks[16];
    uint8_t used;
} inode_t;

/* Finds and returns pointer to inode in inode table based off name. */
inode_t *find_inode(inode_t table[], const char *name);

/* Creates and returns pointer to inode in inode table. */
inode_t *create_inode(inode_t table[], const char *name);