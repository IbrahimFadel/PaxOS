#include "string.h"

char *strncpy(char *destination, const char *source, size_t num) {
  size_t size = strnlen(source, num);
  if (size != num) memset(destination + size, '\0', num - size);
  return memcpy(destination, source, size);
}
