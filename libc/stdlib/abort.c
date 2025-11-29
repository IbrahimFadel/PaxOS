#if defined(__is_libk)
#include "tty/tty.h"
#endif

#include <stdio.h>
#include <stdlib.h>

__attribute__((__noreturn__)) void abort(void) {
#if defined(__is_libk)
  // TODO: Add proper kernel panic.
  tty_set_colour(vga_char_colour(VGA_COLOR_RED, VGA_COLOR_BLACK));
  printf("kernel: panic: abort()\n");
  hlt();
#else
  // TODO: Abnormally terminate the process as if by SIGABRT.
  printf("abort()\n");
#endif
  for (;;) {
  }
  __builtin_unreachable();
}
