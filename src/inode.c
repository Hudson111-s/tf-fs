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
            strcpy(table[i].name, name);
            table[i].used = 1;
            return &table[i];
        }
    }
    return NULL;
}

int free_inode(inode_t table[], const char *name) {
    for (int i = 0; i < MAX_FILES; i++) {
        if (table[i].used && strcmp(table[i].name, name) == 0) {
            memset(&table[i], 0, sizeof(inode_t));
            return 0;
        }
    }
    return -1;
}
