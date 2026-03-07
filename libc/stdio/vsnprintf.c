#include "stdio.h"
#include <string.h>

int vsnprintf(char *buf, size_t size, const char *fmt, va_list args) {
  size_t pos = 0;
  int written = 0;

  while (*fmt) {
    if (*fmt != '%') {
      if (pos + 1 < size) buf[pos] = *fmt;

      pos++;
      written++;
      fmt++;
      continue;
    }

    fmt++; // skip %

    switch (*fmt) {
    case 'c': {
      char c = (char)va_arg(args, int);

      if (pos + 1 < size) buf[pos] = c;

      pos++;
      written++;
      break;
    }

    case 's': {
      const char *s = va_arg(args, const char *);
      while (*s) {
        if (pos + 1 < size) buf[pos] = *s;

        pos++;
        written++;
        s++;
      }
      break;
    }

    case 'd': {
      int v = va_arg(args, int);

      char tmp[32];
      int i = 0;
      int neg = 0;

      if (v < 0) {
        neg = 1;
        v = -v;
      }

      do {
        tmp[i++] = '0' + (v % 10);
        v /= 10;
      } while (v);

      if (neg) tmp[i++] = '-';

      while (i--) {
        if (pos + 1 < size) buf[pos] = tmp[i];

        pos++;
        written++;
      }

      break;
    }

    case 'x': {
      unsigned int v = va_arg(args, unsigned int);

      char tmp[32];
      int i = 0;

      do {
        int d = v & 0xF;
        tmp[i++] = d < 10 ? '0' + d : 'a' + d - 10;
        v >>= 4;
      } while (v);

      while (i--) {
        if (pos + 1 < size) buf[pos] = tmp[i];

        pos++;
        written++;
      }

      break;
    }

    case '%': {
      if (pos + 1 < size) buf[pos] = '%';

      pos++;
      written++;
      break;
    }

    default: break;
    }

    fmt++;
  }

  if (size > 0) {
    if (pos >= size)
      buf[size - 1] = '\0';
    else
      buf[pos] = '\0';
  }

  return written;

  // char *buf = 0;
  // // int result = vasprintf(&buf, format, ap);
  // int result = snprintf(buf, n, format, ap);

  // if (!buf) return -1;
  // if (result < 0) {
  //   // free(buf);
  //   return -1;
  // }

  // result = strlen(buf);
  // if (n > 0) {
  //   if ((long)n > result)
  //     memcpy(s, buf, result + 1);
  //   else {
  //     memcpy(s, buf, n - 1);
  //     s[n - 1] = 0;
  //   }
  // }
  // // free(buf);
  // return result;
}
