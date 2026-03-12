#ifndef KERNEL_FS_RAMFS_RAMFS_H
#define KERNEL_FS_RAMFS_RAMFS_H

#include "fs/fs.h"
#include <stdint.h>
#include <stdbool.h>

typedef struct {
  char name[KCONFIG_RAMFS_MAX_NAME_LEN];
  uint8_t *data;
  uint32_t size;
  bool used;
} ramfs_file_t;

void ramfs_init(void);
fd_t ramfs_create(const char *name, uint8_t *data, uint32_t size);
ramfs_file_t *ramfs_find(const char *name);

#endif
