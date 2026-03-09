#include "unistd.h"

int stdin = 0;
int stdout = 1;
int stderr = 2;

int write(int fd, const void *buf, size_t count) {
  int ret;
  __asm__ volatile("int $0x80" : "=a"(ret) : "a"(1), "b"(fd), "c"(buf), "d"(count) : "memory");
  return ret;
}
