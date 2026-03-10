#ifndef KERNEL_FS_FS_H
#define KERNEL_FS_FS_H

#include "stdbool.h"
#include <stdint.h>

typedef int fd_t;
typedef void *file_t;

typedef struct {
  file_t file;
  uint32_t offset;
  bool used;
} fd_entry_t;

#endif
