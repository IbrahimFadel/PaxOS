#include "mem/vmm.h"
#include "i386/cpu.h"
#include "i386/mmap.h"
#include "i386/mmap_config.h"
#include "logging/logging.h"
#include "mem/page_table.h"
#include "mem/pmm.h"
#include "utils.h"
#include <assert.h>
#include <stdint.h>
#include <string.h>

#define KERNEL_VA_ALLOC_START (ALIGN_UP((uintptr_t)ld_kernel_end, PAGE_SIZE))

void *vmm_alloc_pages(size_t num_pages) {
  void *va_base = vmm_find_free_va_range(num_pages);
  assert(va_base);

  for (size_t i = 0; i < num_pages; i++) {
    void *pa = pmm_alloc_page();
    assert(pa);
    void *va = (uint8_t *)va_base + i * PAGE_SIZE;
    vmm_map_page(kernel_page_dir, va, pa, PAGE_RW);
  }

  return va_base;
}

void vmm_free_pages(void *va_base, size_t num_pages) {
  for (size_t i = 0; i < num_pages; i++) {
    void *va = (uint8_t *)va_base + i * PAGE_SIZE;
    vmm_unmap_page(kernel_page_dir, va);
  }
}

void vmm_map_page(page_table_t pt, void *va, void *pa, uint32_t flags) {
  LOGT("vmm: map_page: table = 0x%x, va = 0x%x, pa = 0x%x, flags = 0x%x\n", pt, va, pa, flags);
  assert(ALIGNED(va, PAGE_SIZE));
  uint32_t l1_idx = vmm_l1_idx((uintptr_t)va);
  uint32_t l0_idx = vmm_l0_idx((uintptr_t)va);

  if (!(pt[l1_idx] & PAGE_PRESENT)) {
    void *page_pa = pmm_alloc_page();
    void *page_va = vmm_pa_to_va(page_pa);
    memset(page_va, 0x0, PAGE_SIZE);
    uint32_t pde_flags = PAGE_PRESENT | PAGE_RW;
    if (flags & PTE_USER) { pde_flags |= PTE_USER; }
    pt[l1_idx] = (uint32_t)page_pa | pde_flags;
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

void *vmm_find_free_va_range(size_t num_pages) {
  size_t consecutive = 0;
  void *range_start = NULL;

  uintptr_t va = ALIGN_UP(KERNEL_VA_ALLOC_START, PAGE_SIZE);

  while (va < UINT32_MAX - PAGE_SIZE) {
    uint32_t l1_idx = vmm_l1_idx(va);
    uint32_t l0_idx = vmm_l0_idx(va);

    // all 1024 entries are not present, so we can just skip the entire thing
    if (!(kernel_page_dir[l1_idx] & PAGE_PRESENT)) {
      if (consecutive == 0) range_start = (void *)va;
      consecutive += PAGE_TABLE_SIZE - l0_idx;
      va += (PAGE_TABLE_SIZE - l0_idx) * PAGE_SIZE;
    } else {
      page_table_t pt = vmm_pa_to_va((void *)(kernel_page_dir[l1_idx] & ~0xFFF));

      if (!(pt[l0_idx] & PAGE_PRESENT)) {
        if (consecutive == 0) range_start = (void *)va;
        consecutive++;
        va += PAGE_SIZE;
      } else {
        consecutive = 0;
        range_start = NULL;
        va += PAGE_SIZE;
      }
    }

    if (consecutive >= num_pages) return range_start;
  }

  return NULL;
}

void *vmm_pa_to_va(void *pa) { return (void *)((uintptr_t)pa + KERNEL_VA); }

void *vmm_va_to_pa(void *va) { return (void *)((uintptr_t)va - KERNEL_VA); }
