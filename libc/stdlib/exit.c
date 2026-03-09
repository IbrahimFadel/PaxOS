#ifndef __is_libk
  #include "stdlib.h"

void exit(int code) {
  int ret;
  __asm__ volatile("int $0x80" : : "a"(0), "b"(0) :);
}
#endif
