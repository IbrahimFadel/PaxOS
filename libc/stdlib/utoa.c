#include <stdbool.h>
#include <string.h>

char *utoa(unsigned int value, char *str, int base) {
  char *rc = str;
  char *ptr;
  char *low;

  if (base < 2 || base > 36) {
    *str = '\0';
    return str;
  }

  ptr = str;

  do {
    int digit = value % base;
    *ptr++ = (digit < 10) ? digit + '0' : digit - 10 + 'a';
    value /= base;
  } while (value);

  *ptr-- = '\0';

  // reverse the string
  for (low = str; low < ptr; low++, ptr--) {
    char tmp = *low;
    *low = *ptr;
    *ptr = tmp;
  }

  return rc;
}
