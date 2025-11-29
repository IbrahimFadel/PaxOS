#ifndef __KERNEL_I386_CPU_H__
#define __KERNEL_I386_CPU_H__

#include <stdint.h>

__attribute__((always_inline)) static inline void set_sp(void *sp) {
  __asm__ __volatile__("mov %0, %%esp" ::"m"(sp) :);
}

__attribute__((noreturn)) static inline void hlt(void) {
  __asm__ __volatile__("cli\n\t"
                       "1:\n\t"
                       "hlt\n\t"
                       "jmp 1b\n\t"
                       :
                       :
                       : "memory");
  __builtin_unreachable();
}

__attribute__((always_inline)) static inline void
set_pdbr(const void *page_directory) {
  __asm__ __volatile__("mov %0, %%cr3" ::"r"(page_directory) :);
}

__attribute__((always_inline)) static inline uint32_t get_cr0(void) {
  uint32_t cr0;
  __asm__ __volatile__("mov %%cr0, %0" : "=r"(cr0)::);
  return cr0;
}

__attribute__((always_inline)) static inline void set_cr0(uint32_t cr0) {
  __asm__ __volatile__("mov %0, %%cr0" : : "r"(cr0) :);
}

__attribute__((always_inline)) static inline uint32_t get_cr3(void) {
  uint32_t cr3;
  __asm__ __volatile__("mov %%cr0, %0" : "=r"(cr3)::);
  return cr3;
}

__attribute__((always_inline)) static inline void set_cr3(uint32_t cr3) {
  __asm__ __volatile__("mov %0, %%cr0" : : "r"(cr3) : "memory");
}

#endif
