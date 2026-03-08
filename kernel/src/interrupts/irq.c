#include "interrupts/irq.h"
#include "i386/cpu.h"
#include "logging/logging.h"
#include <assert.h>

static irq_t irqs[NUM_IRQS];

void register_irq(irqn_t irqn, irq_t irq) {
  assert(irqn >= 0);
  assert(irqn < NUM_IRQS);
  irqs[irqn] = irq;
}

void irq_handler(trap_frame_t *tf) {
  irqn_t irqn = tf->int_no - 32;
  LOGT("irq_handler: irq = %d, eip=%x\n", irqn, tf->eip);

  assert(irqn >= 0);
  assert(irqn < NUM_IRQS);

  if (irqs[irqn]) {
    irqs[irqn](tf);
  } else {
    LOGE("irq_handler: no handler, halting...");
    for (;;) { hlt(); }
  }
}
