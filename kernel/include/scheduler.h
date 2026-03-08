#ifndef KERNEL_SCHEDULER_H
#define KERNEL_SCHEDULER_H

#include "proc.h"

void scheduler_init(void);
void schedule(void);
void scheduler_add(proc_t *proc);
void scheduler_remove(proc_t *proc);

void timer_irq(trap_frame_t *tf);

#endif
