#ifndef KERNEL_IPC_SPINLOCK_H
#define KERNEL_IPC_SPINLOCK_H

#include "proc.h"
#include <stdatomic.h>

typedef struct {
  atomic_int locked;
  cpu_id_t cpu_id;
} spinlock_t;

spinlock_t spinlock_init(void);
void spinlock_lock(spinlock_t *lock);
void spinlock_unlock(spinlock_t *lock);

#endif
