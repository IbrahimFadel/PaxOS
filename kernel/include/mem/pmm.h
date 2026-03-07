#ifndef KERNEL_MEM_PMM_H
#define KERNEL_MEM_PMM_H

#include "multiboot2/multiboot2.h"
#define ALIGNED(ADDR, ALIGNMENT) (((uint32_t)(ADDR)) % (ALIGNMENT) == 0)
#define BOOTSTRAP_MAP_BASE       0xD0000000

void pmm_init(const boot_info_t *boot_info);
void *pmm_alloc_page(void);
void pmm_free(void *p);

#endif
