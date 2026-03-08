#include "tasks/task2.h"
#include <stdint.h>
#include <stdio.h>

void task2(void) {
  printf("task2 entry\n");
  volatile uint64_t i = 0;
  for (;;) {
    if (i % 0x100000) { printf("task2 tick\n"); }
    i++;
  }
}
