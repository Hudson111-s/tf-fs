#include <stdio.h>

#include "superblock.h"
#include "bitmap.h"
#include "inode.h"
#include "tf-fs.h"

/* Init functions. */
int init_fs(FILE *disk);
int init_superblock(FILE *disk);
int init_bitmap(FILE *disk);
int init_inode(FILE *disk);

/* Reading utils. */
int read_superblock(FILE *disk, superblock_t *sb);
int read_bitmap(FILE *disk, uint8_t bitmap[]);
int read_inode(FILE *disk, inode_t table[]);

/* Other. */
int mount_fs(FILE *disk, fs_t *fs);
int sync_fs(fs_t *fs);
