#include "fs/ramfs/ramfs.h"
#include <string.h>

static ramfs_file_t files[KCONFIG_RAMFS_MAX_NUM_FILES];

fd_t ramfs_create(const char *name, uint8_t *data, uint32_t size) {
  for (int i = 0; i < KCONFIG_RAMFS_MAX_NUM_FILES; i++) {
    if (files[i].used) continue;
    strncpy(files[i].name, name, KCONFIG_RAMFS_MAX_NAME_LEN);
    files[i].data = data;
    files[i].size = size;
    files[i].used = true;
    return i;
  }
  return -1;
}

ramfs_file_t *ramfs_find(const char *name) {
  for (int i = 0; i < KCONFIG_RAMFS_MAX_NUM_FILES; i++)
    if (files[i].used && strcmp(files[i].name, name) == 0) return &files[i];
  return NULL;
}
