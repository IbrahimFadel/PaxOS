#ifndef KERNEL_GDT_H
#define KERNEL_GDT_H

#include <stdint.h>

#define GDT_ALIGNMENT 8

typedef struct __attribute__((packed)) {
  uint16_t size;
  uint32_t offset;
} gdtr_t;

typedef uint64_t segment_descriptor_t;
typedef segment_descriptor_t descriptor_table_t[];

#define SEGMENT_DESCRIPTOR_ACCESS_BYTE_NOT_ACCESSED ((uint64_t)0 << 40)
#define SEGMENT_DESCRIPTOR_ACCESS_BYTE_ACCESSED     ((uint64_t)1 << 40)

#define SEGMENT_DESCRIPTOR_ACCESS_BYTE_NO_READ  ((uint64_t)0 << 41)
#define SEGMENT_DESCRIPTOR_ACCESS_BYTE_READ     ((uint64_t)1 << 41)
#define SEGMENT_DESCRIPTOR_ACCESS_BYTE_NO_WRITE ((uint64_t)0 << 41)
#define SEGMENT_DESCRIPTOR_ACCESS_BYTE_WRITE    ((uint64_t)1 << 41)

#define SEGMENT_DESCRIPTOR_ACCESS_BYTE_DIRECTION_UP     ((uint64_t)0 << 42)
#define SEGMENT_DESCRIPTOR_ACCESS_BYTE_DIRECTION_DOWN   ((uint64_t)1 << 42)
#define SEGMENT_DESCRIPTOR_ACCESS_BYTE_CONFORM_DPL_RING ((uint64_t)0 << 42)
#define SEGMENT_DESCRIPTOR_ACCESS_BYTE_CONFORM_LEQ      ((uint64_t)1 << 42)

#define SEGMENT_DESCRIPTOR_ACCESS_BYTE_NO_EXEC ((uint64_t)0 << 43)
#define SEGMENT_DESCRIPTOR_ACCESS_BYTE_EXEC    ((uint64_t)1 << 43)

#define SEGMENT_DESCRIPTOR_ACCESS_BYTE_TYPE_SYSTEM ((uint64_t)0 << 44)
#define SEGMENT_DESCRIPTOR_ACCESS_BYTE_TYPE_CODE   ((uint64_t)1 << 44)
#define SEGMENT_DESCRIPTOR_ACCESS_BYTE_TYPE_DATA   ((uint64_t)1 << 44)

#define SEGMENT_DESCRIPTOR_ACCESS_BYTE_RING_0 ((uint64_t)0 << 45)
#define SEGMENT_DESCRIPTOR_ACCESS_BYTE_RING_3 ((uint64_t)3 << 45)

#define SEGMENT_DESCRIPTOR_ACCESS_BYTE_NOT_PRESENT ((uint64_t)0 << 47)
#define SEGMENT_DESCRIPTOR_ACCESS_BYTE_PRESENT     ((uint64_t)1 << 47)

#define SEGMENT_DESCRIPTOR_ACCESS_BYTE(BYTE) ((uint64_t)((BYTE) & 0xFF) << 40)

#define SEGMENT_DESCRIPTOR_FLAGS_NOT_LONG_MODE ((uint64_t)0 << 53)
#define SEGMENT_DESCRIPTOR_FLAGS_LONG_MODE     ((uint64_t)1 << 53)

#define SEGMENT_DESCRIPTOR_FLAGS_SIZE_16BIT ((uint64_t)0 << 54)
#define SEGMENT_DESCRIPTOR_FLAGS_SIZE_32BIT ((uint64_t)1 << 54)

#define SEGMENT_DESCRIPTOR_FLAGS_GRANULARITY_BYTE ((uint64_t)0 << 55)
#define SEGMENT_DESCRIPTOR_FLAGS_GRANULARITY_PAGE ((uint64_t)1 << 55)

#define SEGMENT_DESCRIPTOR_FLAGS(FLAGS) ((uint64_t)((FLAGS) & 0xF) << 52)

#define SEGMENT_DESCRIPTOR_BASE_LOWER(ADDR) ((uint64_t)((ADDR) & 0xFFFF) << 16)
#define SEGMENT_DESCRIPTOR_BASE_MID(ADDR)   (((uint64_t)((ADDR) >> 16) & 0xFF) << 32)
#define SEGMENT_DESCRIPTOR_BASE_UPPER(ADDR) (((uint64_t)((ADDR) >> 24) & 0xFF) << 56)
#define SEGMENT_DESCRIPTOR_BASE(ADDR)                                      \
  (SEGMENT_DESCRIPTOR_BASE_LOWER(ADDR) | SEGMENT_DESCRIPTOR_BASE_MID(ADDR) \
   | SEGMENT_DESCRIPTOR_BASE_UPPER(ADDR))

#define SEGMENT_DESCRIPTOR_LIMIT_LOWER(ADDR) ((uint64_t)(ADDR) & 0xFFFF)
#define SEGMENT_DESCRIPTOR_LIMIT_UPPER(ADDR) (((uint64_t)((ADDR) >> 16) & 0xF) << 48)
#define SEGMENT_DESCRIPTOR_LIMIT(ADDR)                                          \
  (SEGMENT_DESCRIPTOR_LIMIT_LOWER(ADDR) | SEGMENT_DESCRIPTOR_LIMIT_UPPER(ADDR))

#define GDT_SELECTOR(i)     ((i) << 3)
#define GDT_KERNEL_CODE_IDX 1
#define GDT_KERNEL_DATA_IDX 2
#define GDT_USER_CODE_IDX   3
#define GDT_USER_DATA_IDX   4
#define GDT_TSS_IDX         5

#define KERNEL_RING 0
#define USER_RING   3

extern __attribute__((aligned(GDT_ALIGNMENT))) segment_descriptor_t gdt[6];

void gdt_install_tss(void);
void gdt_set_gdtr(void);

#endif
