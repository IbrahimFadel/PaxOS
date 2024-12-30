#include <kernel/rsdp.h>
#include <kernel/tty.h>
#include <stdio.h>
#include <string.h>

void rsdp_init() {
  struct facp_t *facp = acpi_get_rsdp();
  printf("Found FACP: %d\n", (int)facp);
}

void kmain(void) {
  tty_init();
  printf("Pax OS v%d.%d\n", 0, 1);

  rsdp_init();
}
