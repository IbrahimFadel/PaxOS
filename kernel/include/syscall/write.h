#ifndef KERNEL_SYSCALL_WRITE_H
#define KERNEL_SYSCALL_WRITE_H

#include <stddef.h>

int write(int fd, const void *buf, size_t count);

#endif
