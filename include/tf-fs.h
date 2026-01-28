#pragma once

#include <stdio.h>
#include <stdint.h>
#include "superblock.h"
#include "bitmap.h"
#include "inode.h"

inode_t *create_file(inode_t table[], const char *name);

/* 
    This inits file that were created with init data. 
    Should not be used to append to a file.
*/
int write_file(FILE *disk, superblock_t *sb, int8_t bitmap[], inode_t table[], inode_t *inode, uint8_t data[], size_t size);

/* Reads file into out buffer. */
int read_file(FILE *disk, inode_t *inode, uint8_t out[]);

/* Reads file by file name into out buffer. */
int read_file_bname(FILE *disk, inode_t table[], uint8_t out[], const char *name);
