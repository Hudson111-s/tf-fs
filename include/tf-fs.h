#pragma once

#include <stdio.h>
#include <stdint.h>
#include "superblock.h"
#include "bitmap.h"
#include "inode.h"

int write_file(FILE *disk, superblock_t *sb, int8_t bitmap[], inode_t table[], inode_t *inode, uint8_t data[], size_t size);

int write_file_bname(FILE *disk, superblock_t *sb, int8_t bitmap[], inode_t table[], const char *name, uint8_t data[], size_t size);

int read_file(FILE *disk, inode_t *inode, uint8_t out[]);

int read_file_bname(FILE *disk, inode_t table[], uint8_t out[], const char *name);
