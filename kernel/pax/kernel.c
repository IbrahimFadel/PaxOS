#include <pax/acpi.h>
#include <pax/boot_info.h>
#include <pax/interrupt.h>
#include <pax/memory.h>
#include <pax/multiboot.h>
#include <pax/protect.h>
#include <pax/tty.h>
#include <stdio.h>
#include <string.h>
#include <sys/io.h>

// Temporary workaround until i fix ACPI
void shutdown(void) { outw(0x2000, 0x604); }

void kmain(u32 magic, u32 mbi_phys_addr) {
  printf("PaxOS booted\n");
  if (mb_verify_boot(magic, mbi_phys_addr) == FAILURE) {
    shutdown();
  }

  // mb_init(mbi_phys_addr);

  // gdt_init();
  // idt_init();

  // u32 phys_alloc_start = (mbi_phys_addr + 0xFFF) & ~0xFFF;
  // mem_init(mbi_phys_addr, phys_alloc_start);
  // printf("memory allocation done\n");

  tty_init();
  char buf[16];
  sprintf(buf, "Pax OS v%d.%d\n", 0, 1);
  tty_writestring(buf);

  // printf("hi\n");

  // mem_init(multiboot_info);
  // initAcpi();

  // acpiPowerOff();

  // shutdown();
}
