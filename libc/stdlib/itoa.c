#include <stdbool.h>
#include <string.h>

char* itoa(int value, char* str, int base) {
  int i = 0;
  bool neg = false;

  if (value == 0) {
    str[i++] = '0';
    str[i] = '\0';
    return str;
  }

  if (value < 0 && base == 10) {
    neg = true;
    value = -value;
  }

  while (value != 0) {
    int rem = value % base;
    str[i++] = (rem > 9) ? (rem - 10) + 'a' : rem + '0';
    value = value / base;
  }

  if (neg) str[i++] = '-';

  str[i] = '\0';

  strrev(str);

  return str;
}