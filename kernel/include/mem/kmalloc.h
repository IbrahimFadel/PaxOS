#ifndef KERNEL_MEM_KMALLOC_H
#define KERNEL_MEM_KMALLOC_H

#include <stddef.h>

void *kmalloc(size_t size);
void kfree(void *p);

#endif
