#include "pit.h"
#include "logging/logging.h"
#include "sys/io.h"
#include <assert.h>

#define PIT_CMD       0x43
#define PIT_CH0       0x40
#define PIT_FREQUENCY 1193182

void pit_init(uint32_t hz) {
  uint32_t divisor = PIT_FREQUENCY / hz;
  assert(divisor <= 0xFFFF);

  outb(PIT_CMD, 0x36);
  outb(PIT_CH0, divisor & 0xFF);
  outb(PIT_CH0, (divisor >> 8) & 0xFF);

  LOGD("pit_init: hz=%d divisor=%d\n", hz, divisor);
}
