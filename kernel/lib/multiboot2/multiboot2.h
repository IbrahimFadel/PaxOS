#ifndef __KERNEL_LIB_MULTIBOOT2_H__
#define __KERNEL_LIB_MULTIBOOT2_H__

#include <stdint.h>

#define MULTIBOOT2_MAGIC 0xE85250D6
#define MULTIBOOT2_NUM_TAGS 1

typedef enum {
  MULTIBOOT2_ARCH_32BIT_PROT = 0x0,
  MULTIBOOT2_ARCH_32BIT_MIPS = 0x4,
} multiboot2_arch_e;

typedef struct __attribute__((packed)) {
  uint16_t type;
  uint16_t flags;
  uint32_t size;
} multiboot2_tag_s;

typedef struct __attribute__((packed)) {
  uint32_t magic;
  uint32_t architecture;
  uint32_t header_length;
  uint32_t checksum;
  multiboot2_tag_s tags[MULTIBOOT2_NUM_TAGS];
} multiboot2_header_s;

#define MULTIBOOT2_HEADER_CHECKSUM(__MULTIBOOT2_ARCH__, __HEADER_SIZE__)       \
  (-(MULTIBOOT2_MAGIC + __MULTIBOOT2_ARCH__ + __HEADER_SIZE__))

#endif
