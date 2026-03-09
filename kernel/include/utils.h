#ifndef KERNEL_UTILS_H
#define KERNEL_UTILS_H

#define ALIGN_UP(x, align)   (((x) + (align) - 1) & ~((align) - 1))
#define ALIGN_DOWN(x, align) ((x) & ~((align) - 1))

#define MIN(a, b) ((a) < (b) ? (a) : (b))

#endif
