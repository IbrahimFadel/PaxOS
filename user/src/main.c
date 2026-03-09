#include <stddef.h>
#include <stdlib.h>

int main(void);

static inline int write(int fd, const void *buf, size_t len) {
  int ret;
  __asm__ volatile("int $0x80" : "=a"(ret) : "a"(4), "b"(fd), "c"(buf), "d"(len) : "memory");
  return ret;
}

void _start(void) { exit(main()); }

int main(void) {
  const char *msg = "hello from ring 3\n"; // .rodata — totally fine now
  write(1, msg, 18);
  return 0;
}
