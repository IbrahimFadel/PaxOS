#include <limits.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static bool print(char* dest, int idx, const char* data, size_t length) {
  const unsigned char* bytes = (const unsigned char*)data;
  for (size_t i = 0; i < length; i++) {
    dest[idx + i] = bytes[i];
  }
  // if (dest[idx + i] = (bytes[i]) == EOF) return false;
  return true;
}

int sprintf(char* str, const char* format, ...) {
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
      if (!print(str, written, format, amount)) return -1;
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
      if (!print(str, written, &c, sizeof(c))) return -1;
      written++;
    } else if (*format == 's') {
      format++;
      char* str = va_arg(parameters, char*);
      size_t len = strlen(str);
      if (maxrem < len) {
        // TODO: Set errno to EOVERFLOW.
        return -1;
      }
      if (!print(str, written, str, len)) return -1;
      written += len;
    } else if (*format == 'd') {
      format++;
      int n = (int)va_arg(parameters, int);
      char n_str[11];
      itoa(n, n_str, 10);
      size_t len = strlen(n_str);
      if (maxrem < len) {
        // TODO: Set errno to EOVERFLOW.
        return -1;
      }
      if (!print(str, written, n_str, len)) return -1;
      written += len;
    } else {
      format = format_begun_at;
      size_t len = strlen(format);
      if (maxrem < len) {
        // TODO: Set errno to EOVERFLOW.
        return -1;
      }
      if (!print(str, written, format, len)) return -1;
      written += len;
      format += len;
    }
  }

  va_end(parameters);
  return written;
}