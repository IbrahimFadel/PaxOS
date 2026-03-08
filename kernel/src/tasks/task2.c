#include "tasks/task2.h"
#include <stdint.h>
#include <stdio.h>

volatile uint32_t task2_count = 0;

void task2(void) {
  printf("task2 entry\n");
  int i = 0;
  for (;;) {
    if (i % 0x100000 == 0) { task2_count++; }
    i++;
  }
}
