#include "mem/bootstrap.h"
#include "i386/cpu.h"
#include "i386/mmap.h"
#include "logging/logging.h"
#include "mem/pmm.h"
#include "mem/vmm.h"
#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

static uint8_t bootstrap_page_tables[NUM_BOOTSTRAP_PAGES * PAGE_SIZE] PAGE_ALIGNED;
static size_t bootstrap_pt_index = 0;

void *bootstrap_mem_alloc_page(void) {
  assert(bootstrap_pt_index < NUM_BOOTSTRAP_PAGES);
  void *p = &bootstrap_page_tables[bootstrap_pt_index * PAGE_SIZE];
  bootstrap_pt_index++;
  return p;
}

void bootstrap_mem_map_page(void *va, void *pa) {
  LOGT("vmm: bootstrap_mem_map_page: va = 0x%x, pa = 0x%x\n", va, pa);
  assert(ALIGNED(va, PAGE_SIZE));
  uint32_t l1 = vmm_l1_idx((uintptr_t)va);
  uint32_t l0 = vmm_l0_idx((uintptr_t)va);

  uint32_t *pd = (uint32_t *)0xFFFFF000;

  if (!(pd[l1] & PAGE_PRESENT)) {
    uint32_t *new_pt = bootstrap_mem_alloc_page();
    uintptr_t pt_pa = (uintptr_t)new_pt - KERNEL_VA;
    memset(new_pt, 0, PAGE_SIZE);
    pd[l1] = pt_pa | PAGE_PRESENT | PAGE_RW;
  }

  uint32_t *pt = (uint32_t *)(0xFFC00000 + l1 * PAGE_SIZE);
  pt[l0] = ((uintptr_t)pa & ~0xFFF) | PAGE_PRESENT | PAGE_RW;

  invlpg(va);
}

void bootstrap_mem_unmap_page(void *va) {
  LOGT("vmm: bootstrap_mem_unmap_page: va = 0x%x\n", va);
  assert(ALIGNED(va, PAGE_SIZE));
  uint32_t l1_idx = vmm_l1_idx((uintptr_t)va);
  uint32_t l0_idx = vmm_l0_idx((uintptr_t)va);

  uint32_t *pt = (uint32_t *)0xFFFFF000;

  if (!(pt[l1_idx] & PAGE_PRESENT)) return;

  pt = vmm_pa_to_va((void *)(pt[l1_idx] & ~0xFFF));
  if (!(pt[l0_idx] & PAGE_PRESENT)) return;

  void *pa = (void *)(pt[l0_idx] & ~0xFFF);
  pt[l0_idx] = 0;

  bootstrap_pt_index--;

  invlpg(va);
}
