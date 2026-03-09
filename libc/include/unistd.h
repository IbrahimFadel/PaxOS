#ifndef KLIBC_UNISTD_H
#define KLIBC_UNISTD_H

#include <stddef.h>

int write(int fd, const void *buf, size_t count);

#endif
