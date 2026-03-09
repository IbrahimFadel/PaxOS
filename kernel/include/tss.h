#ifndef KERNEL_TSS_H
#define KERNEL_TSS_H

#include "gdt.h"
#include "logging/logging.h"
#include <stdint.h>

#define TSS_SECTION __attribute__((section(".tss")))

extern uint8_t ld_tss_base[];

typedef struct __attribute__((packed)) {
  uint16_t link;
  uint16_t reserved0;

  uint32_t esp0;
  uint16_t ss0;
  uint16_t reserved1;

  uint32_t esp1;
  uint16_t ss1;
  uint16_t reserved2;

  uint32_t esp2;
  uint16_t ss2;
  uint16_t reserved3;

  uint32_t cr3;
  uint32_t eip;
  uint32_t eflags;

  uint32_t eax;
  uint32_t ecx;
  uint32_t edx;
  uint32_t ebx;

  uint32_t esp;
  uint32_t ebp;

  uint32_t esi;
  uint32_t edi;

  uint16_t es;
  uint16_t reserved4;

  uint16_t cs;
  uint16_t reserved5;

  uint16_t ss;
  uint16_t reserved6;

  uint16_t ds;
  uint16_t reserved7;

  uint16_t fs;
  uint16_t reserved8;

  uint16_t gs;
  uint16_t reserved9;

  uint16_t ldtr;
  uint16_t reserved10;

  uint16_t reserved11;
  uint16_t iopb;
  // uint32_t ssp;
} tss_t;

_Static_assert(sizeof(tss_t) == 0x68, "tss_t size");

extern tss_t tss;

void tss_init(void);
__attribute__((naked)) void tss_load_segment_registers(void);

static inline void tss_set_kernel_stack(uint32_t esp0) {
  tss.esp0 = esp0;
  LOGT("tss.esp0 = 0x%x tss.ss0 = 0x%x\n", tss.esp0, tss.ss0);
}

static inline void tss_load(void) { __asm__ volatile("ltr %0" ::"r"(GDT_SELECTOR(GDT_TSS_IDX))); }

#endif
