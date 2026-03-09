#include "syscall/write.h"
#include "i386/mmap_config.h"
#include "logging/logging.h"
#include "stdbool.h"
#include <pax/tty.h>

#define STDIN  0
#define STDOUT 1
#define STDERR 2

static bool uptr_valid(const void *ptr, size_t len);

int sys_write(int fd, const void *buf, size_t count) {
  LOGI("syscall: write: fd = %d, buf = 0x%x, count = %d\n", fd, buf, count);
  if (!uptr_valid(buf, count)) {
    LOGE("buf invalid\n");
    return -1;
  }

  if (fd == STDOUT) {
    tty_writestring((const char *)buf);
    return count;
  }
  return -1;
}

static bool uptr_valid(const void *ptr, size_t len) {
  uint32_t start = (uint32_t)ptr;
  uint32_t end = start + len;

  if (end < start) return false;
  if (end > KERNEL_VA) return false;

  return true;
}
