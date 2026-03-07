#ifndef KERNEL_INTRINSICS_H
#define KERNEL_INTRINSICS_H

#if defined __has_builtin
  #if __has_builtin(__builtin_ctz)
    #define builtin_ctz(n) __builtin_ctz(n)
  #endif
#endif
#ifndef builtin_ctz
  #define builtin_ctz(n) (void)n
#endif

#endif
