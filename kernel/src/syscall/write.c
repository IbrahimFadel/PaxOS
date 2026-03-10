#include "syscall/write.h"
#include "logging/logging.h"
#include "syscall/syscall.h"
#include <pax/tty.h>

#define STDIN  0
#define STDOUT 1
#define STDERR 2

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
