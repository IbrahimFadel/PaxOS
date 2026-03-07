#ifndef KERNEL_MEM_VMM_H
#define KERNEL_MEM_VMM_H

#include <stdint.h>
#include "page_table.h"

extern pte_t kernel_page_dir[];

void vmm_map_page(page_table_t pt, void *va, void *pa, uint32_t flags);
void vmm_unmap_page(page_table_t pt, void *va);
void *vmm_pa_to_va(void *pa);

static inline uint32_t vmm_l1_idx(uintptr_t va) { return va >> 22; }
static inline uint32_t vmm_l0_idx(uintptr_t va) { return (va >> 12) & 0x3FF; }

#endif
