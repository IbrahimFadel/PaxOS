#include <pax/tty.h>
#include <stdint.h>
#include <stdio.h>

void kmain(uint32_t mb2_magic, uint32_t mb2_info_pa) {
  tty_init();
  tty_writestring("hello, world\n");
  tty_writestring("hi\n");

  printf("magic: 0x%x\n", mb2_magic);
  printf("ptr: 0x%x\n", mb2_info_pa);

  for (;;) {
  }
}
