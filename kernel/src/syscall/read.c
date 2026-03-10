#include "syscall/read.h"
#include "fs/fs.h"
#include "fs/ramfs/ramfs.h"
#include "logging/logging.h"
#include "scheduler.h"
#include "syscall/syscall.h"
#include "utils.h"
#include <string.h>

int sys_read(fd_t fd, void *buf, uint32_t len) {
  if (!uptr_valid(buf, len)) return -1;
  if (fd < 0 || fd >= KCONFIG_MAX_FDS) {
    LOGI("sys_read: invalid fd %d\n", fd);
    return -1;
  }
  if (!current_proc->fds[fd].used) {
    LOGI("sys_read: fd %d not allocated\n", fd);
    return -1;
  }

  fd_entry_t *entry = &current_proc->fds[fd];

  // for now imma do this
  file_t *file = entry->file;
  ramfs_file_t *ramfs_file = (ramfs_file_t *)file;

  uint32_t remaining = ramfs_file->size - entry->offset;
  uint32_t n = MIN(len, remaining);
  memcpy(buf, ramfs_file->data + entry->offset, n);
  entry->offset += n;
  return n;
}
