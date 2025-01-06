#ifndef _MEMORY_H
#define _MEMORY_H

#include <pax/multiboot.h>

void *get_physaddr(void *virtualaddr);
void map_page(void *physaddr, void *virtualaddr, unsigned int flags);

#define KERNEL_START 0xC0000000
#define KERNEL_HEAP 0xD0000000
#define NUM_ENTRIES 1024

extern u32 page_dir[NUM_ENTRIES];

void mem_init(u32 mem_high, u32 phys_alloc_start);

// void mem_init(struct multiboot_info_t *multiboot_info);

#endif
