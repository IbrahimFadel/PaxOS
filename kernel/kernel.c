#include "i386/mmap.h"
#include "multiboot2/multiboot2.h"
#include "stdlib.h"
#include <pax/tty.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sys/io.h>

#define MBI_VA 0xD0000000

void map_phys(uint32_t pa, uint32_t va);

typedef struct {
  uint32_t total_size;
  uint32_t reserved;
} multiboot2_info;

void kmain(uint32_t mb2_magic, uint32_t mb2_info_pa) {
  com1_init();
  tty_init();
  tty_writestring("hello, world\n");

  if (mb2_magic != 0x36d76289) {
    tty_writestring("multiboot2 magic number incorrect\n");
    abort();
  }

  printf("magic: 0x%x\n", mb2_magic);
  printf("ptr: 0x%x\n", mb2_info_pa);

  // Map MBI physical page to a known VA before touching it
  map_phys(mb2_info_pa & ~0xFFF, MBI_VA);
  multiboot2_info *mbi = (multiboot2_info *)(MBI_VA + (mb2_info_pa & 0xFFF));

  printf("0x%x\n", mbi->total_size);
  printf("0x%x\n", mbi->reserved);

  for (;;) {
  }
}

void map_phys(uint32_t pa, uint32_t va) {
  uint32_t pd_idx = va >> 22;
  uint32_t pt_idx = (va >> 12) & 0x3FF;

  // access PD itself via recursive mapping (PD[1023] -> PD)
  uint32_t *pd = (uint32_t *)0xFFFFF000;

  if (!(pd[pd_idx] & PAGE_PRESENT)) {
    // allocate a page for the new page table
    // for now, use a static buffer since you don't have a heap yet
    static uint8_t pt_storage[PAGE_TABLE_SIZE * PTE_SIZE]
        __attribute__((aligned(PAGE_SIZE)));
    uint32_t pt_pa = (uint32_t)pt_storage - KERNEL_VA;

    memset(pt_storage, 0, sizeof(pt_storage));
    pd[pd_idx] = pt_pa | PAGE_PRESENT | PAGE_RW;
  }

  uint32_t *pt = (uint32_t *)(0xFFC00000 + pd_idx * PAGE_SIZE);
  pt[pt_idx] = (pa & ~0xFFF) | PAGE_PRESENT | PAGE_RW;

  asm volatile("invlpg %0" ::"m"(*(char *)va) : "memory");
}
