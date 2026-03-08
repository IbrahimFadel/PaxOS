#ifndef KERNEL_INTERRUPTS_IRQ_H
#define KERNEL_INTERRUPTS_IRQ_H

#include "interrupts/idt.h"

#define NUM_IRQS 32

typedef void (*irq_t)(trap_frame_t *tf);

typedef enum {
  IRQ0 = 0,
  IRQ1 = 1,
  IRQ2 = 2,
  IRQ3 = 3,
  IRQ4 = 4,
  IRQ5 = 5,
  IRQ6 = 6,
  IRQ7 = 7,
  IRQ8 = 8,
  IRQ9 = 9,
  IRQ10 = 10,
  IRQ11 = 11,
  IRQ12 = 12,
  IRQ13 = 13,
  IRQ14 = 14,
  IRQ15 = 15,
  IRQ16 = 16,
  IRQ17 = 17,
  IRQ18 = 18,
  IRQ19 = 19,
  IRQ20 = 20,
  IRQ21 = 21,
  IRQ22 = 22,
  IRQ23 = 23,
  IRQ24 = 24,
  IRQ25 = 25,
  IRQ26 = 26,
  IRQ27 = 27,
  IRQ28 = 28,
  IRQ29 = 29,
  IRQ30 = 30,
  IRQ31 = 31,
} irqn_t;

#define TIMER_IRQ 0

void register_irq(irqn_t irqn, irq_t irq);

#endif

