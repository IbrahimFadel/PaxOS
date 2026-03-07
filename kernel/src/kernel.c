#include "gdt.h"
#include "i386/cpu.h"
#include "i386/mmap.h"
#include "idt.h"
#include "multiboot2/multiboot2.h"
#include "pic.h"
#include "stdlib.h"
#include "tss.h"
#include <pax/tty.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sys/io.h>

#define MBI_VA      0xD0000000
#define SHSTRTAB_VA 0xD0002000

void map_phys(uint32_t pa, uint32_t va);

void kmain(uint32_t mb2_magic, uint32_t mb2_info_pa) {
  com1_init();
  tty_init();
  tty_writestring("PaxOS v0.0.1\n");

  if (mb2_magic != MULTIBOOT2_SUCCESSFUL_BOOT_MAGIC) {
    tty_writestring("multiboot2 magic number incorrect\n");
    abort();
  }
  if (mb2_info_pa & 0x7) {
    tty_writestring("multiboot2 info not aligned\n");
    abort();
  }

  cli();
  gdt_install_tss();
  gdt_set_gdtr();
  tss_init();
  tss_load_segment_registers();
  tss_load();

  idt_init();
  idt_load();
  pic_remap(0x20, 0x28);
  pic_unmask_all();
  sti();

  map_phys(mb2_info_pa & ~0xFFF, MBI_VA);
  map_phys((mb2_info_pa & ~0xFFF) + PAGE_SIZE, MBI_VA + PAGE_SIZE);
  uint32_t mbi_addr = (MBI_VA + (mb2_info_pa & 0xFFF));
  multiboot2_info_parse((multiboot2_boot_info_t *)mbi_addr);

  for (;;) {}
}

void map_phys(uint32_t pa, uint32_t va) {
  uint32_t pd_idx = va >> 22;
  uint32_t pt_idx = (va >> 12) & 0x3FF;

  // access PD itself via recursive mapping (PD[1023] -> PD)
  uint32_t *pd = (uint32_t *)0xFFFFF000;

  if (!(pd[pd_idx] & PAGE_PRESENT)) {
    // allocate a page for the new page table
    // for now, use a static buffer since you don't have a heap yet
    static uint8_t pt_storage[PAGE_TABLE_SIZE * PTE_SIZE] __attribute__((aligned(PAGE_SIZE)));
    uint32_t pt_pa = (uint32_t)pt_storage - KERNEL_VA;

    memset(pt_storage, 0, sizeof(pt_storage));
    pd[pd_idx] = pt_pa | PAGE_PRESENT | PAGE_RW;
  }

  uint32_t *pt = (uint32_t *)(0xFFC00000 + pd_idx * PAGE_SIZE);
  pt[pt_idx] = (pa & ~0xFFF) | PAGE_PRESENT | PAGE_RW;

  __asm__ __volatile__("invlpg %0" ::"m"(*(char *)va) : "memory");
}

__attribute__((noreturn)) void panic(const char *expr, const char *file, int line,
                                     const char *func) {
  cli();
  tty_writestring("\n=== KERNEL PANIC ===\n");
  printf("\n=== KERNEL PANIC ===\n");
  printf("assertion failed: %s\n", expr);
  printf("location: %s:%d (%s)\n", file, line, func);
  for (;;) {
    hlt();
  }
}
