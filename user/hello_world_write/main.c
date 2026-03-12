#include <unistd.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

int main(void) {
  const char *msg = "hello from ring3\n";
  write(stdout, msg, strlen(msg));
  for (;;) {}
  return 0;
}
