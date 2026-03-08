#include "tasks/task1.h"
#include <stdint.h>
#include <stdio.h>

volatile uint32_t task1_count = 0;

void task1(void) {
  printf("task1 entry\n");
  int i = 0;
  for (;;) {
    if (i % 0x100000 == 0) { task1_count++; }
    i++;
  }
}
