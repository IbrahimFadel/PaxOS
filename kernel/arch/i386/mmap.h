#ifndef KERNEL_ARCH_i386_MMAP_H
#define KERNEL_ARCH_i386_MMAP_H

#include <stdint.h>
#include "mmap_config.h" // IWYU pragma: export;

extern uint8_t ld_kernel_end[];

extern uint32_t _user_text_start; // VMA = 0x00400000 (user virtual address)
extern uint32_t _user_text_end;   // VMA = 0x00400000 + size
extern uint32_t _user_text_phys;  // LMA = physical address to map from

#endif
