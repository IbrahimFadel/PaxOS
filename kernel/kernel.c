#include <pax/tty.h>
#include <stdint.h>
#include <stdio.h>
#include <sys/io.h>

void kmain(uint32_t mb2_magic, uint32_t mb2_info_pa) {
  com1_init();
  tty_init();
  tty_writestring("hello, world\n");

  printf("magic: 0x%x\n", mb2_magic);
  printf("ptr: 0x%x\n", mb2_info_pa);

  for (;;) {
  }
}
