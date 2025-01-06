#include <pax/tty.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

// TODO: Don't hard code the stack guard

#if UINT32_MAX == UINTPTR_MAX
#define STACK_CHK_GUARD 0xe2dee396
#else
#define STACK_CHK_GUARD 0x595e9fbd94fda766
#endif

const uintptr_t __stack_chk_guard = STACK_CHK_GUARD;

// __attribute__((noreturn)) void __stack_chk_fail(void) {
// #if __STDC_HOSTED__
//   abort();
// #elif __is_pax_os_kernel
//   tty_set_colour(vga_char_colour(VGA_COLOR_RED, VGA_COLOR_BLACK));
//   printf("Stack smashing detected\n");
//   abort();
// #endif
// }
