#ifndef _PROTECT_H
#define _PROTECT_H

#include <sys/types.h>

#define PL0 0
#define PL1 1
#define PL2 2
#define PL3 3

#define DESC_TY_SYS 0
#define DESC_TY_CODE_DATA 1

#define DESC_NO_EXE 0
#define DESC_EXE 1

#define DESC_DIR_UP 0
#define DESC_DIR_DOWN 1

#define DESC_CONF_DPL 0
#define DESC_CONF_DPL_OR_LOWER 1

#define DESC_NO_RD 0
#define DESC_RD 1

#define DESC_NO_WR 0
#define DESC_WR 1

#define PRESENT(x) (x << 7)
#define DPL(x) (x << 5)
#define TY(x) (x << 4)
#define EXE(x) (x << 3)
#define DC(x) (x << 2)
#define RW(x) (x << 1)
#define A(x) x

#define GRAN_BYTE 0
#define GRAN_PAGE 1

#define SIZE_16 0
#define SIZE_32 1

#define GRAN(x) (x << 3)
#define SIZE(x) (x << 2)
#define LONG(x) (x << 1)

// 1100
#define FLAGS GRAN(GRAN_PAGE) | SIZE(SIZE_32) | LONG(0)

// Kernel Code
// 10011010
#define ACCESS_KERNEL_CODE                                        \
  PRESENT(1) | DPL(PL0) | TY(DESC_TY_CODE_DATA) | EXE(DESC_EXE) | \
      DC(DESC_CONF_DPL) | RW(DESC_RD) | A(0)

// 10010010
// Kernel Data
#define ACCESS_KERNEL_DATA                                           \
  PRESENT(1) | DPL(PL0) | TY(DESC_TY_CODE_DATA) | EXE(DESC_NO_EXE) | \
      DC(DESC_DIR_UP) | RW(DESC_WR) | A(0)

// 11111010
// User Code
#define ACCESS_USER_CODE                                          \
  PRESENT(1) | DPL(PL3) | TY(DESC_TY_CODE_DATA) | EXE(DESC_EXE) | \
      DC(DESC_CONF_DPL) | RW(DESC_RD) | A(0)

// 11110010
// User Data
#define ACCESS_USER_DATA                                             \
  PRESENT(1) | DPL(PL3) | TY(DESC_TY_CODE_DATA) | EXE(DESC_NO_EXE) | \
      DC(DESC_CONF_DPL) | RW(DESC_WR) | A(0)

struct gdt_desc_t {
  u16 size;
  u32 offset;
} __attribute__((packed));

struct seg_desc_t {
  u16 limit;
  u16 base_low;
  u8 base_middle;
  u8 access;
  u8 flags;
  u8 base_high;
} __attribute__((packed));

void gdt_init(void);

#endif
