#include <sys/io.h>

#if defined(__is_libk)
  #include <pax/tty.h>
#endif

int putchar(int c) {
#if defined(__is_libk)
  // wait for transmit buffer to be empty
  while (!(inb(COM1 + 5) & 0x20)) {}
  outb(COM1, c);
#else
  // TODO: Implement stdio and the write system call.
#endif
  return c;
}
