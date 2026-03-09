#include "syscall/syscall.h"
#include <stddef.h>

__attribute__((section(".user_text"))) static inline int syscall_write(int fd, const void *buf,
                                                                       size_t len) {
  int ret;
  __asm__ volatile("int $0x80"
                   : "=a"(ret)
                   : "a"(SYS_WRITE), // eax = syscall number
                     "b"(fd),        // ebx = fd
                     "c"(buf),       // ecx = buf
                     "d"(len)        // edx = len
                   : "memory");
  return ret;
}

__attribute__((section(".user_text"))) __attribute__((noinline)) void user_function(void) {
  // char msg[]
  // = {'h', 'e', 'l', 'l', 'o', ' ', 'f', 'r', 'o', 'm', ' ', 'r', 'i', 'n', 'g', ' ', '3', '\n'};
  char msg[18];
  msg[0] = 'h';
  msg[1] = 'e';
  msg[2] = 'l';
  msg[3] = 'l';
  msg[4] = 'o';
  msg[5] = ' ';
  msg[6] = 'f';
  msg[7] = 'r';
  msg[8] = 'o';
  msg[9] = 'm';
  msg[10] = ' ';
  msg[11] = 'r';
  msg[12] = 'i';
  msg[13] = 'n';
  msg[14] = 'g';
  msg[15] = ' ';
  msg[16] = '3';
  msg[17] = '\n';
  int fd = 1;
  int len = sizeof(msg);

  int ret;
  __asm__ volatile("int $0x80"
                   : "=a"(ret)
                   : "a"(SYS_WRITE), // eax = syscall number
                     "b"(fd),        // ebx = fd
                     "c"(msg),       // ecx = buf
                     "d"(len)        // edx = len
                   : "memory");

  __asm__ volatile("int $0x80"
                   :
                   : "a"(1), // SYS_EXIT as immediate
                     "b"(0)
                   :);

  for (;;);
}
