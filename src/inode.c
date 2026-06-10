#include <string.h>

#include "inode.h"
#include "block.h"

inode_t *find_inode(inode_t table[], const char *name) {
    for (int i = 0; i < MAX_FILES; i++) {
        if (table[i].used && strcmp(table[i].name, name) == 0) {
            return &table[i];
        }
    }
    return NULL;
}

inode_t *create_inode(inode_t table[], const char *name) {
    for (int i = 0; i < MAX_FILES; i++) {
        if (!table[i].used) {
            memset(&table[i], 0, sizeof(inode_t));
            strncpy(table[i].name, name, MAX_FILE_NAME);
            table[i].used = 1;
            return &table[i];
        }
    }
    return NULL;
}

void free_inode(inode_t *inode) {
    memset(inode, 0, sizeof(inode_t));
}
