#include "tss.h"
#include "gdt.h"
#include "i386/mmap.h"

TSS_SECTION __attribute__((aligned(TSS_ALIGNMENT))) tss_t tss;

extern uint8_t kernel_stack_top[KERNEL_STACK_SIZE];

void tss_init(void) {
  tss.ss0 = GDT_SELECTOR(GDT_KERNEL_DATA_IDX);
  tss.esp0 = (uint32_t)kernel_stack_top;
  tss.iopb = sizeof(tss_t);
}

__attribute__((naked)) void tss_load_segment_registers(void) {
  __asm__ __volatile__("reload_segments:\n"
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

void tss_load(void) {
  __asm__ __volatile__("mov %0, %%ax \n"
                       "ltr %%ax"
                       :
                       : "i"(GDT_SELECTOR(GDT_TSS_IDX))
                       : "ax");
}
