#include "tasks/idle.h"
#include "i386/cpu.h"

void idle_task(void) {
  for (;;) { hlt(); }
}
