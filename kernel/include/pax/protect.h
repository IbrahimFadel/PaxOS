#ifndef _PROTECT_H
#define _PROTECT_H

#include <sys/types.h>

#define GDT_SIZE 6
#define TSS_IDX (GDT_SIZE - 1)

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
#define FLAGS (GRAN(GRAN_PAGE) | SIZE(SIZE_32) | LONG(0))
#define FLAGS_TSS (GRAN(GRAN_BYTE) | SIZE(SIZE_16) | LONG(0))

// Kernel Code
#define ACCESS_KERNEL_CODE                                        \
  PRESENT(1) | DPL(PL0) | TY(DESC_TY_CODE_DATA) | EXE(DESC_EXE) | \
      DC(DESC_CONF_DPL) | RW(DESC_RD) | A(0)

// 10010010
#define ACCESS_KERNEL_DATA                                           \
  PRESENT(1) | DPL(PL0) | TY(DESC_TY_CODE_DATA) | EXE(DESC_NO_EXE) | \
      DC(DESC_DIR_UP) | RW(DESC_WR) | A(0)

// 11111010
#define ACCESS_USER_CODE                                          \
  PRESENT(1) | DPL(PL3) | TY(DESC_TY_CODE_DATA) | EXE(DESC_EXE) | \
      DC(DESC_CONF_DPL) | RW(DESC_RD) | A(0)

// 11110010
#define ACCESS_USER_DATA                                             \
  PRESENT(1) | DPL(PL3) | TY(DESC_TY_CODE_DATA) | EXE(DESC_NO_EXE) | \
      DC(DESC_CONF_DPL) | RW(DESC_WR) | A(0)

// 11101001
#define ACCESS_TSS                                          \
  PRESENT(1) | DPL(PL3) | TY(DESC_TY_SYS) | EXE(DESC_EXE) | \
      DC(DESC_CONF_DPL) | RW(DESC_NO_RD) | A(1)

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

struct tss_entry_t {
  u32 prev_tss;
  u32 esp0;
  u32 ss0;
  u32 esp1;
  u32 ss1;
  u32 esp2;
  u32 ss2;
  u32 cr3;
  u32 eip;
  u32 eflags;
  u32 eax;
  u32 ecx;
  u32 edx;
  u32 ebx;
  u32 esp;
  u32 ebp;
  u32 esi;
  u32 edi;
  u32 es;
  u32 cs;
  u32 ss;
  u32 ds;
  u32 fs;
  u32 gs;
  u32 ldtr;
  u32 iopb;
  u32 ssp;
} __attribute__((packed));

void gdt_init(void);

#endif
