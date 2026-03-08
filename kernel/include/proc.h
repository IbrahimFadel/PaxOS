#ifndef KERNEL_PROC_H
#define KERNEL_PROC_H

#include "stdbool.h"
#include <stdint.h>

typedef uint32_t cpu_id_t;

typedef struct {
  cpu_id_t id;
  int num_interrupt_offs;
  bool interrupts_enabled;
} cpu_t;

cpu_t *cpu_current(void);

#endif
