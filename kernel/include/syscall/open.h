#ifndef KERNEL_SYSCALL_OPEN_H
#define KERNEL_SYSCALL_OPEN_H

#include "fs/fs.h"

fd_t sys_open(const char *path, int flags);

#endif
