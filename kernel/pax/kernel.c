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
  printf("booted\n");
  if (mb_verify_boot(magic, mbi_phys_addr) == FAILURE) {
    shutdown();
  }

  gdt_init();
  idt_init();

  // tty_init();
  // char buf[16];
  // sprintf(buf, "Pax OS v%d.%d\n", 0, 1);
  // tty_writestring(buf);

  struct mb_tag_t *tag = (struct mb_tag_t *)(mbi_phys_addr + 8);
  int x = tag->type;
  switch (x) {
    default:
      printf("%d\n", x);
      break;
  }
}
