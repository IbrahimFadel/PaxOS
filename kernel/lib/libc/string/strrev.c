#include <string.h>

char* strrev(char* str) {
  size_t length = strlen(str);
  int start = 0;
  int end = length - 1;
  while (start < end) {
    char temp = str[start];
    str[start] = str[end];
    str[end] = temp;
    end--;
    start++;
  }
  return str;
}