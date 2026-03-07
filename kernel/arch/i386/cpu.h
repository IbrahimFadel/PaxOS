#ifndef KERNEL_I386_CPU_H
#define KERNEL_I386_CPU_H

#include "logging/logging.h"
#include <stdint.h>

static inline void cli(void) { __asm__ __volatile__("cli" :::); }

static inline void sti(void) { __asm__ __volatile__("sti" :::); }

static inline void hlt(void) { __asm__ __volatile__("hlt" :::); }

static inline void invlpg(void *p) {
  LOGT("invpg: 0x%x\n", p);
  __asm__ __volatile__("invlpg (%0)" ::"r"(p) : "memory");
}

#endif
