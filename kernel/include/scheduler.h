#ifndef KERNEL_SCHEDULER_H
#define KERNEL_SCHEDULER_H

#include "proc.h"

extern proc_t *current_proc;

void scheduler_init(void);
void schedule(void);
void scheduler_add(proc_t *proc);
void scheduler_remove(proc_t *proc);

void timer_irq(trap_frame_t *tf);

#endif
