#ifndef KERNEL_IPC_SPINLOCK_H
#define KERNEL_IPC_SPINLOCK_H

typedef struct {
  unsigned locked;
#ifdef KCONFIG_DEBUG_EN
#endif
} spinlock_t;

#endif
