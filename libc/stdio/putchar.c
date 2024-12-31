#include <sys/io.h>

static const uint16_t COM1 = 0x3F8;

// #if defined(__is_libk)
// #include <kernel/tty.h>
// #endif

int putchar(int c) {
#if defined(__is_libk)
  outb((char)c, COM1);
#else
  // TODO: Implement stdio and the write system call.
#endif
  return c;
}
