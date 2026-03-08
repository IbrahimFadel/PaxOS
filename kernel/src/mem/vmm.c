#include "mem/vmm.h"
#include "i386/cpu.h"
#include "i386/mmap.h"
#include "logging/logging.h"
#include "mem/page_table.h"
#include "mem/pmm.h"
#include <assert.h>
#include <stdint.h>
#include <string.h>

void vmm_map_page(page_table_t pt, void *va, void *pa, uint32_t flags) {
  LOGT("vmm: map_page: table = 0x%x, va = 0x%x, pa = 0x%x, flags = 0x%x\n", pt, va, pa, flags);
  assert(ALIGNED(va, PAGE_SIZE));
  uint32_t l1_idx = vmm_l1_idx((uintptr_t)va);
  uint32_t l0_idx = vmm_l0_idx((uintptr_t)va);

  if (!(pt[l1_idx] & PAGE_PRESENT)) {
    void *page_pa = pmm_alloc_page();
    void *page_va = vmm_pa_to_va(page_pa);
    memset(page_va, 0x0, PAGE_SIZE);
    pt[l1_idx] = (uint32_t)page_pa | PAGE_PRESENT | PAGE_RW;
  }

  void *l0_pa = (void *)(pt[l1_idx] & ~0xFFF);
  page_table_t l0_va = vmm_pa_to_va(l0_pa);
  l0_va[l0_idx] = ((uintptr_t)pa & ~0xFFF) | flags | PAGE_PRESENT;

  invlpg(va);
}

void vmm_unmap_page(page_table_t pt, void *va) {
  LOGT("vmm: unmap_page: table = 0x%x, va = 0x%x\n", pt, va);
  assert(ALIGNED(va, PAGE_SIZE));
  uint32_t l1_idx = vmm_l1_idx((uintptr_t)va);
  uint32_t l0_idx = vmm_l0_idx((uintptr_t)va);
  if (!(pt[l1_idx] & PAGE_PRESENT)) return;

  pt = vmm_pa_to_va((void *)(pt[l1_idx] & ~0xFFF));
  if (!(pt[l0_idx] & PAGE_PRESENT)) return;

  void *pa = (void *)(pt[l0_idx] & ~0xFFF);
  pt[l0_idx] = 0;
  pmm_free(pa);

  invlpg(va);
}

void *vmm_pa_to_va(void *pa) { return (void *)((uintptr_t)pa + KERNEL_VA); }

void *vmm_va_to_pa(void *va) { return (void *)((uintptr_t)va - KERNEL_VA); }
