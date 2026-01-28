#include <stdio.h>
#include "superblock.h"
#include "bitmap.h"
#include "inode.h"
typedef struct fs_t{
    superblock_t sb;
    uint8_t bitmap[BITMAP_SIZE];
    inode_t table[MAX_FILES];
} fs_t;

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
int sync_fs(FILE *disk, superblock_t *sb, uint8_t bitmap[], inode_t table[]);
