#include "stdio.h"

int snprintf(char *s, size_t n, const char *format, ...) {
  int result;
  va_list ap;
  va_start(ap, format);
  result = vsnprintf(s, n, format, ap);
  va_end(ap);
  return result;
}
