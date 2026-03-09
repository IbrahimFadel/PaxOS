#ifndef KERNEL_MEM_PAGE_TABLE_H
#define KERNEL_MEM_PAGE_TABLE_H

#include <stdint.h>

typedef uint32_t pte_t;
typedef pte_t *page_table_t;

#define PTE_PRESENT 1
#define PTE_RW      (1 << 1)
#define PTE_USER    (1 << 2)

#endif
