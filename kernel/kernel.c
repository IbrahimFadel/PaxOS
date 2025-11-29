#include "memory/paging.h"
#include "tty/tty.h"

void kmain(void) {
  tty_init();
  tty_writestring("hello, world\n");
  for (;;) {
  }
}
