#ifndef __KERNEL_I386_CPU_H__
#define __KERNEL_I386_CPU_H__

#include "logging/logging.h"
#include <stdint.h>

static inline void set_sp(void *sp) { __asm__ __volatile__("mov %0, %%esp" ::"m"(sp) :); }

static inline void set_pdbr(const void *page_directory) {
  __asm__ __volatile__("mov %0, %%cr3" ::"r"(page_directory) :);
}

static inline uint32_t get_cr0(void) {
  uint32_t cr0;
  __asm__ __volatile__("mov %%cr0, %0" : "=r"(cr0)::);
  return cr0;
}

static inline void set_cr0(uint32_t cr0) { __asm__ __volatile__("mov %0, %%cr0" : : "r"(cr0) :); }

static inline uint32_t get_cr3(void) {
  uint32_t cr3;
  __asm__ __volatile__("mov %%cr0, %0" : "=r"(cr3)::);
  return cr3;
}

static inline void set_cr3(uint32_t cr3) {
  __asm__ __volatile__("mov %0, %%cr3" : : "r"(cr3) : "memory");
}

static inline void cli(void) { __asm__ __volatile__("cli" :::); }

static inline void sti(void) { __asm__ __volatile__("sti" :::); }

static inline void hlt(void) { __asm__ __volatile__("hlt" :::); }

static inline void invlpg(void *p) {
  LOGT("invpg: 0x%x\n", p);
  __asm__ __volatile__("invlpg (%0)" ::"r"(p) : "memory");
}

#endif
