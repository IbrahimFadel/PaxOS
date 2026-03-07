#include "gdt.h"
#include "i386/cpu.h"
#include "i386/mmap.h"
#include "interrupts/idt.h"
#include "mem/pmm.h"
#include "mem/vmm.h"
#include "multiboot2/multiboot2.h"
#include "pic.h"
#include "stdlib.h"
#include "tss.h"
#include <pax/tty.h>
#include <stdint.h>
#include <stdio.h>
#include <sys/io.h>
#include <assert.h>

void kmain(uint32_t mb2_magic, uint32_t mbi_pa) {
  com1_init();
  tty_init();
  if (mb2_magic != MULTIBOOT2_SUCCESSFUL_BOOT_MAGIC) { abort(); }
  if (mbi_pa & 0x7) { abort(); }

  gdt_install_tss();
  gdt_set_gdtr();
  tss_init();
  tss_load_segment_registers();
  tss_load();

  idt_init();
  idt_load();
  pic_remap(0x20, 0x28);
  sti();

  tty_writestring("PaxOS v0.0.1\n");

  uint32_t mbi_va = (BOOTSTRAP_MAP_BASE + (mbi_pa & 0xFFF));
  boot_info_t boot_info;

  multiboot2_map((const multiboot2_boot_info_t *)mbi_pa);
  multiboot2_info_parse(&boot_info, (const multiboot2_boot_info_t *)mbi_va);
  pmm_init(&boot_info);
  multiboot2_unmap();

  uintptr_t test_va = 0xC1000000;
  void *page = pmm_alloc_page();
  vmm_map_page(kernel_page_dir, (void *)test_va, page, PAGE_RW);

  uint32_t *ptr = (uint32_t *)test_va;
  *ptr = 0xDEADBEEF;
  assert(*ptr == 0xDEADBEEF);

  int x = 1 / 0;

  for (;;) {}
}

__attribute__((noreturn)) void panic(const char *expr, const char *file, int line,
                                     const char *func) {
  cli();
  tty_writestring("\n=== KERNEL PANIC ===\n");
  printf("\n=== KERNEL PANIC ===\n");
  printf("assertion failed: %s\n", expr);
  printf("location: %s:%d (%s)\n", file, line, func);
  for (;;) {
    hlt();
  }
}
