#include "syscall/open.h"
#include "fs/ramfs/ramfs.h"
#include "logging/logging.h"
#include "scheduler.h"
#include "syscall/syscall.h"

int sys_open(const char *path, int flags) {
  if (!uptr_valid(path, 1)) return -1;

  ramfs_file_t *file = ramfs_find(path);
  if (!file) {
    LOGI("sys_open: could not find file: %s\n", path);
    return -1;
  }

  for (int i = 3; i < KCONFIG_MAX_FDS; i++) {
    if (current_proc->fds[i].used) continue;
    current_proc->fds[i].file = file;
    current_proc->fds[i].offset = 0;
    current_proc->fds[i].used = true;
    LOGI("sys_open: fd = %d\n", i);
    return i;
  }
  LOGI("sys_open: `KCONFIG_MAX_FDS` exceeded, could not allocate file\n");
  return -1;
}
