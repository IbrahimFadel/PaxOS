#ifndef KLIBC_STDLIB_H
#define KLIBC_STDLIB_H

#include <sys/cdefs.h>

#ifdef __cplusplus
extern "C" {
#endif

  __attribute__((__noreturn__)) void abort(void);

  char *itoa(int value, char *str, int base);
  char *utoa(unsigned int value, char *str, int base);

#ifndef __is_klibc
  void exit(int code);
#endif

#ifdef __cplusplus
}
#endif

#endif
