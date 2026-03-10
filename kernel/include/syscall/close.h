#ifndef KERNEL_SYSCALL_CLOSE_H
#define KERNEL_SYSCALL_CLOSE_H

#include "fs/fs.h"

int sys_close(fd_t fd);

#endif
