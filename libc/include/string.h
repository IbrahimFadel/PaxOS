#ifndef KLIBC_STRING_H
#define KLIBC_STRING_H

#include <stddef.h>
#include <sys/cdefs.h>

#ifdef __cplusplus
extern "C" {
#endif

  int memcmp(const void *, const void *, size_t);
  void *memcpy(void *__restrict, const void *__restrict, size_t);
  void *memmove(void *, const void *, size_t);
  void *memset(void *, int, size_t);
  size_t strlen(const char *);
  int strcmp(const char *s1, const char *s2);
  char *strcpy(char *destination, const char *source);
  char *strncpy(char *destination, const char *source, size_t num);
  size_t strnlen(const char *s, size_t maxlen);
  char *strrev(char *str);

#ifdef __cplusplus
}
#endif

#endif
