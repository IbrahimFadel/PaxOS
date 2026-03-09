#include "tss.h"
#include "gdt.h"
#include "i386/mmap.h"
#include <stdint.h>
#include <string.h>

TSS_SECTION __attribute__((aligned(TSS_ALIGNMENT))) tss_t tss;

void tss_init(void) {
  memset(&tss, 0, sizeof(tss));
  tss.ss0 = GDT_SELECTOR(GDT_KERNEL_DATA_IDX);
  tss.iopb = sizeof(tss_t);
}

__attribute__((naked)) void tss_load_segment_registers(void) {
  __asm__ __volatile__(
    "reload_segments:\n"
    "ljmp $0x08, $1f\n"
    "1:\n"
    "mov $0x10, %%ax\n"
    "mov %%ax, %%ds\n"
    "mov %%ax, %%es\n"
    "mov %%ax, %%fs\n"
    "mov %%ax, %%gs\n"
    "mov %%ax, %%ss\n"
    "ret\n"
    :
    :
    : "ax");
}
