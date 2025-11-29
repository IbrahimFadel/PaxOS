#ifndef __KERNEL_LIB_MEMORY_PAGING_H__
#define __KERNEL_LIB_MEMORY_PAGING_H__

#include <stdint.h>

#define PAGE_DIRECTORY_SIZE 0x400
#define PAGE_TABLE_SIZE 0x400

#define PAGE_SIZE 0x1000

typedef uint32_t page_directory_entry_t;
typedef uint32_t page_table_entry_t;

typedef page_directory_entry_t
    __attribute__((aligned(PAGE_SIZE))) page_directory_t[PAGE_DIRECTORY_SIZE];
typedef page_table_entry_t
    __attribute__((aligned(PAGE_SIZE))) page_table_t[PAGE_TABLE_SIZE];

#endif
