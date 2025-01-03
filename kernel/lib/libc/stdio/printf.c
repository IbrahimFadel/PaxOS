#include <limits.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static bool print(const char* data, size_t length) {
  const unsigned char* bytes = (const unsigned char*)data;
  for (size_t i = 0; i < length; i++)
    if (putchar(bytes[i]) == EOF) return false;
  return true;
}

// size_t int_buf_size(int n, int radix) {
//   if (radix == 10) {
//     if (n < 0) n = (n == INT_MIN) ? INT_MAX : -n;
//     if (n < 10) return 1;
//     if (n < 100) return 2;
//     if (n < 1000) return 3;
//     if (n < 10000) return 4;
//     if (n < 100000) return 5;
//     if (n < 1000000) return 6;
//     if (n < 10000000) return 7;
//     if (n < 100000000) return 8;
//     if (n < 1000000000) return 9;
//     /*      2147483647 is 2^31-1 - add more ifs as needed
//        and adjust this final return as well. */
//     return 10;
//   }
// }

int printf(const char* restrict format, ...) {
  va_list parameters;
  va_start(parameters, format);

  int written = 0;

  while (*format != '\0') {
    size_t maxrem = INT_MAX - written;

    if (format[0] != '%' || format[1] == '%') {
      if (format[0] == '%') format++;
      size_t amount = 1;
      while (format[amount] && format[amount] != '%') amount++;
      if (maxrem < amount) {
        // TODO: Set errno to EOVERFLOW.
        return -1;
      }
      if (!print(format, amount)) return -1;
      format += amount;
      written += amount;
      continue;
    }

    const char* format_begun_at = format++;

    if (*format == 'c') {
      format++;
      char c = (char)va_arg(parameters, int /* char promotes to int */);
      if (!maxrem) {
        // TODO: Set errno to EOVERFLOW.
        return -1;
      }
      if (!print(&c, sizeof(c))) return -1;
      written++;
    } else if (*format == 's') {
      format++;
      const char* str = va_arg(parameters, const char*);
      size_t len = strlen(str);
      if (maxrem < len) {
        // TODO: Set errno to EOVERFLOW.
        return -1;
      }
      if (!print(str, len)) return -1;
      written += len;
    } else if (*format == 'd') {
      format++;
      int n = (int)va_arg(parameters, int);
      char str[11];
      itoa(n, str, 10);
      size_t len = strlen(str);
      if (maxrem < len) {
        // TODO: Set errno to EOVERFLOW.
        return -1;
      }
      if (!print(str, len)) return -1;
      written += len;
    } else {
      format = format_begun_at;
      size_t len = strlen(format);
      if (maxrem < len) {
        // TODO: Set errno to EOVERFLOW.
        return -1;
      }
      if (!print(format, len)) return -1;
      written += len;
      format += len;
    }
  }

  va_end(parameters);
  return written;
}
