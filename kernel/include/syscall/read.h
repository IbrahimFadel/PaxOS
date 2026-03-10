#ifndef KERNEL_SYSCALL_READ_H
#define KERNEL_SYSCALL_READ_H

#include "fs/fs.h"
#include <stdint.h>

int sys_read(fd_t fd, void *buf, uint32_t len);

#endif
