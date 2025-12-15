#include <stdio.h>
#include "superblock.h"
#include "bitmap.h"
#include "inode.h"

int init_fs(FILE *disk);

int init_superblock(FILE *disk);

int init_bitmap(FILE *disk);

int init_inode(FILE *disk);
