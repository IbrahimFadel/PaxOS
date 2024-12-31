#include <kernel/acpi.h>
#include <kernel/serial.h>
#include <kernel/tty.h>
#include <stdio.h>
#include <string.h>

void kmain(void) {
  tty_init();

  char buf[16];
  sprintf(buf, "Pax OS v%d.%d\n", 0, 1);
  tty_writestring(buf);

  printf("hello\n");
}
