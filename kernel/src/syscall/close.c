#include "syscall/close.h"
#include "logging/logging.h"
#include "scheduler.h"

int sys_close(fd_t fd) {
  if (fd < 3 || fd >= KCONFIG_MAX_FDS) {
    LOGI("sys_close: invalid fd %d\n", fd);
    return -1;
  }
  if (!current_proc->fds[fd].used) {
    LOGI("sys_close: fd %d not allocated\n");
    return -1;
  }
  current_proc->fds[fd].used = false;
  return 0;
}
