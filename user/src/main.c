#include "unistd.h"
#include <stddef.h>
#include <stdio.h>

int main(void) {
  const char *msg = "hello from usermode\n";
  write(stdout, msg, 18);
  return 0;
}
