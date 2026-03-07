#ifndef KLIBC_STDIO_H
#define KLIBC_STDIO_H

#include "stdarg.h"
#include <sys/cdefs.h>
#include <stddef.h>

#define EOF (-1)

#ifdef __cplusplus
extern "C" {
#endif

  int sprintf(char *str, const char *format, ...);
  int printf(const char *__restrict, ...);
  int putchar(int);
  int puts(const char *);
  int vsnprintf(char *buf, size_t size, const char *fmt, va_list args);
  int snprintf(char *s, size_t n, const char *format, ...);

#ifdef __cplusplus
}
#endif

#endif
