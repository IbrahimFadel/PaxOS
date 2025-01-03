#include <pax/acpi.h>
#include <pax/interrupt.h>
#include <pax/protect.h>
#include <pax/tty.h>
#include <stdio.h>
#include <string.h>
#include <sys/io.h>

// Temporary workaround until i fix ACPI
void shutdown(void) { outw(0x2000, 0x604); }

void kmain(void) {
  tty_init();

  char buf[16];
  sprintf(buf, "Pax OS v%d.%d\n", 0, 1);
  tty_writestring(buf);

  printf("Booted\n");

  gdt_init();
  idt_init();

  // initAcpi();

  // acpiPowerOff();

  shutdown();
}
