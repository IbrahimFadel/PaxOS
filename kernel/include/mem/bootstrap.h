#ifndef KERNEL_MEM_BOOTSTRAP_H
#define KERNEL_MEM_BOOTSTRAP_H

#define NUM_BOOTSTRAP_PAGES 4

void *bootstrap_mem_alloc_page(void);
void bootstrap_mem_map_page(void *va, void *pa);
void bootstrap_mem_unmap_page(void *va);

#endif
