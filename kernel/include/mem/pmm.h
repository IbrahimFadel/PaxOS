#ifndef KERNEL_MEM_PMM_H
#define KERNEL_MEM_PMM_H

#define ALIGNED(ADDR, ALIGNMENT) ((ADDR) % (ALIGNMENT) == 0)

void pmm_init(void);
void *pmm_alloc_page(void);
void pmm_free(void *p);

#endif
