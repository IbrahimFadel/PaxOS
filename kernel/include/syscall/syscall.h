#ifndef KERNEL_SYSCALL_SYSCALL_H
#define KERNEL_SYSCALL_SYSCALL_H

#include "interrupts/idt.h"
#include "stdbool.h"
#include <stddef.h>

typedef enum {
  SYS_EXIT,
  SYS_WRITE,
  SYS_BRK,
} syscall_t;

void syscall_handler(trap_frame_t *tf);

bool uptr_valid(const void *ptr, size_t len);

#endif
