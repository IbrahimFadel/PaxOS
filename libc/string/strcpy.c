#include <string.h>

char* strcpy(char* destination, const char* source) {
  size_t size = strlen(source);
  for (size_t i = 0; i < size; i++) {
    destination[i] = source[i];
  }
  return destination;
}
