#include "cpu.h"
#include "kernel.h"
#include "memory/map.h"
#include "memory/paging.h"
#include "multiboot2/multiboot2.h"
#include <stdint.h>

void _start(void) __attribute__((noreturn, section(".multiboot2.text")));
static void paging_init(void *tty_buf)
    __attribute__((section(".multiboot2.text")));
static void paging_enable(void) __attribute__((section(".multiboot2.text")));
static void page_table_map_addr(int idx, page_table_entry_t entry)
    __attribute__((section(".multiboot2.text")));
static void page_dir_map_table(int idx, page_directory_entry_t entry)
    __attribute__((section(".multiboot2.text")));
static void enter_higher_half(void)
    __attribute__((section(".multiboot2.text")));
static void page_dir_unmap_table(int idx);
static void tlb_flush(void);

void higher_half(void) __attribute__((noreturn));

#define STACK_SIZE 0x4000 // 16KB
#define STACK_ALIGNMENT 16

// Preset & R/W Permissions
#define PTE(__ADDR__) ((page_table_entry_t)(__ADDR__ & (0xFFFFF000)) | 0b11)

volatile page_directory_t page_dir;
volatile page_directory_t page_table;

uint8_t stack[STACK_SIZE] __attribute__((section(".bootstrap_stack"),
                                         aligned(STACK_ALIGNMENT), used));

void enter_higher_half(void) { asm volatile("jmp *%0" ::"r"(&higher_half)); }

void _start(void) {
  paging_init((void *)0x000B8000);
  paging_enable();
  enter_higher_half();
  __builtin_unreachable();
  hlt();
}

void higher_half(void) {
  page_dir_unmap_table(0);
  tlb_flush();

  set_sp((void *)((uintptr_t)stack + STACK_SIZE));

  kmain();
  hlt();
  __builtin_unreachable();
}

#define KERNEL_VADDR 0xC0000000

static void paging_init(void *tty_buf) {
  uintptr_t kernel_va = KERNEL_VADDR;
  uintptr_t kernel_start_phy = (uintptr_t)&_kernel_start;
  uintptr_t kernel_end_va = (uintptr_t)&_kernel_end;
  uintptr_t kernel_end_phy = kernel_end_va - kernel_va;
  uintptr_t kernel_size = kernel_end_phy - kernel_start_phy;
  uint32_t kernel_pages = (uint32_t)((kernel_size + PAGE_SIZE - 1) / PAGE_SIZE);
  uintptr_t page_table_phy = ((uintptr_t)page_table) - kernel_va;
  uintptr_t page_dir_phy = ((uintptr_t)page_dir) - kernel_va;

  for (uint32_t i = 0; i < PAGE_TABLE_SIZE; i++) {
    page_table_map_addr(i, 0);
  }
  for (uint32_t i = 0; i < kernel_pages; i++) {
    uintptr_t phys = (kernel_start_phy + (uintptr_t)i * PAGE_SIZE);
    page_table_map_addr(i, phys);
  }

  page_table_map_addr(PAGE_TABLE_SIZE - 1, (uintptr_t)tty_buf);

  page_dir_map_table(0, page_table_phy);
  page_dir_map_table(768, page_table_phy);
  page_dir_map_table(PAGE_DIRECTORY_SIZE - 1, page_dir_phy);

  set_pdbr((void *)page_dir_phy);
}

static void paging_enable(void) {
  uint32_t cr0 = get_cr0();
  cr0 |= 0x80000001;
  set_cr0(cr0);
}

static void page_table_map_addr(int idx, page_table_entry_t entry) {
  page_table_entry_t *arr = (page_table_entry_t *)page_table;
  uintptr_t addr = (uintptr_t)arr - KERNEL_VADDR;
  addr += idx * 4;
  *(page_table_entry_t *)addr = PTE(entry);
}

static void page_dir_map_table(int idx, page_directory_entry_t entry) {
  page_directory_entry_t *arr = (page_directory_entry_t *)page_dir;
  uintptr_t addr = (uintptr_t)arr - KERNEL_VADDR;
  addr += idx * 4;
  *(page_directory_entry_t *)addr = PTE(entry);
}

static void page_dir_unmap_table(int idx) { page_dir[idx] = 0x0; }

static void tlb_flush(void) {
  uint32_t cr3 = get_cr3();
  set_cr3(cr3);
}
