#ifndef KLIBC_ASSERT_H
#define KLIBC_ASSERT_H

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __is_libk
  __attribute__((noreturn)) void panic(const char *expr, const char *file, int line,
                                       const char *func);
  #ifdef KCONFIG_ENABLE_ASSERTIONS
    #define assert(EXPR)                                                                    \
      (__builtin_expect(!(EXPR), 0) ? panic(#EXPR, __FILE__, __LINE__, __func__) : (void)0)
  #else
    #define assert(EXPR) ((void)(EXPR))
  #endif

#endif

#ifdef __cplusplus
}
#endif

#endif
