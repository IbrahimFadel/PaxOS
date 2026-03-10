#include "gdt.h"
#include "i386/cpu.h"
#include "i386/mmap.h"
#include "i386/mmap_config.h"
#include "interrupts/idt.h"
#include "mem/kmalloc.h"
#include "mem/pmm.h"
#include "mem/vmm.h"
#include "multiboot2/multiboot2.h"
#include "pic.h"
#include "proc/proc.h"
#include "scheduler.h"
#include "stdlib.h"
#include "tasks/task1.h"
#include "tasks/task2.h"
#include "tss.h"
#include <pax/tty.h>
#include <stdint.h>
#include <stdio.h>
#include <sys/io.h>
#include <assert.h>
#include "kconfig.h" // IWYU pragma: export;

extern void user_function(void);

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

  char os_name[256];
  snprintf(os_name, sizeof(os_name) / sizeof(os_name[0]), "PaxOS v%d.%d.%d\n",
           KCONFIG_VERSION_MAJOR, KCONFIG_VERSION_MINOR, KCONFIG_VERSION_PATCH);
  tty_writestring(os_name);

  // uint32_t mbi_va = (BOOTSTRAP_MAP_BASE + (mbi_pa & 0xFFF));
  // boot_info_t boot_info;

  // multiboot2_map((const multiboot2_boot_info_t *)mbi_pa);
  // multiboot2_info_parse(&boot_info, (const multiboot2_boot_info_t *)mbi_va);
  // pmm_init(&boot_info);
  // multiboot2_unmap();

  // uintptr_t test_va = 0xC1000000;
  // void *page = pmm_alloc_page();
  // vmm_map_page(kernel_page_dir, (void *)test_va, page, PAGE_RW);

  // uint32_t *ptr = (uint32_t *)test_va;
  // *ptr = 0xDEADBEEF;
  // assert(*ptr == 0xDEADBEEF);

  // // vmm_unmap_page(kernel_page_dir, ptr);
  // // *ptr = 0xFF00FF00;

  // kmem_init();

  // void *p0 = kmalloc(PAGE_SIZE);
  // void *p1 = kmalloc(1);
  // void *p2 = kmalloc(50);
  // void *p3 = kmalloc(100);
  // void *p4 = kmalloc(500);
  // void *p5 = kmalloc(PAGE_SIZE * 3 + 30);

  // kfree(p1);
  // void *p6 = kmalloc(1);
  // assert(p1 == p6);

  // kfree(p2);
  // void *p7 = kmalloc(50);
  // assert(p2 == p7);

  // kfree(p5);
  // void *p8 = kmalloc(PAGE_SIZE + 1);
  // assert(p5 == p8);

  // scheduler_init();

  // proc_t *kproc1 = process_create(task1, PRIV_KERNEL);
  // assert(kproc1);

  // proc_t *kproc2 = process_create(task2, PRIV_KERNEL);
  // assert(kproc2);

  // scheduler_add(kproc1);
  // scheduler_add(kproc2);

  // initrd_load(mod->mod_start, mod->mod_end - mod->mod_start);

  // ramfs_file_t *f = ramfs_find("hello.elf");
  // assert(f != NULL);
  // proc_t *proc = elf_load(f->data, f->size);
  // scheduler_add(proc);

  for (;;) {}
}

__attribute__((noreturn)) void panic(const char *expr, const char *file, int line,
                                     const char *func) {
  cli();
  tty_writestring("\n=== KERNEL PANIC ===\n");
  printf("\n=== KERNEL PANIC ===\n");
  printf("assertion failed: %s\n", expr);
  printf("location: %s:%d (%s)\n", file, line, func);
  for (;;) { hlt(); }
}
