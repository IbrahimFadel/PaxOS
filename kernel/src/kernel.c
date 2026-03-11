#include "fs/ramfs/initrd.h"
#include "fs/ramfs/ramfs.h"
#include "gdt.h"
#include "i386/cpu.h"
#include "interrupts/idt.h"
#include "logging/logging.h"
#include "mem/kmalloc.h"
#include "mem/pmm.h"
#include "multiboot2/multiboot2.h"
#include "pic.h"
#include "proc/elf.h"
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

  uint32_t mbi_va = (BOOTSTRAP_MAP_BASE + (mbi_pa & 0xFFF));
  boot_info_t boot_info;

  multiboot2_map((const multiboot2_boot_info_t *)mbi_pa);
  multiboot2_info_parse(&boot_info, (const multiboot2_boot_info_t *)mbi_va);
  pmm_init(&boot_info);
  multiboot2_unmap();

  kmem_init();
  scheduler_init();

  // proc_t *kproc1 = process_create(task1, PRIV_KERNEL);
  // assert(kproc1);

  // proc_t *kproc2 = process_create(task2, PRIV_KERNEL);
  // assert(kproc2);

  // scheduler_add(kproc1);
  // scheduler_add(kproc2);

  ramfs_init();
  initrd_load(boot_info.initrd_module.mod_start, boot_info.initrd_module.mod_end);
  ramfs_file_t *f = ramfs_find("./hello_world_write");
  if (f) {
    proc_t *proc = elf_load(f->data, f->size);
    assert(proc);
    scheduler_add(proc);
  } else {
    LOGE("could not find ELF\n");
  }

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
