#include "tasks/task1.h"
#include <stdint.h>
#include <stdio.h>

void task1(void) {
  printf("task1 entry\n");
  volatile uint64_t i = 0;
  for (;;) {
    if (i % 0x100000) { printf("task1 tick\n"); }
    i++;
  }
}
