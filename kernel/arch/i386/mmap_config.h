#ifndef KERNEL_ARCH_i386_MMAP_CONFIG_H
#define KERNEL_ARCH_i386_MMAP_CONFIG_H

#define KERNEL_VA           0xC0000000
#define KERNEL_OFFSET_START 0x00100000
#define KERNEL_START        (KERNEL_VA + KERNEL_OFFSET_START)

#define KERNEL_STACK_SIZE 0x4000

#define KERNEL_PHY_END (KERNEL_OFFSET_START + 1024 * 1024)

#define STACK_SIZE      0x4000 // 16KB
#define STACK_ALIGNMENT 16

#define VGA_BUF_PHY_ADDR 0x000B8000

#define PAGE_TABLE_SIZE 0x400
#define PAGE_SIZE       0x1000
#define PTE_SIZE        4
#define PAGE_ALIGNED    __attribute__((aligned(PAGE_SIZE)))
#define PAGE_PRESENT    1
#define PAGE_RW         (1 << 1)

#define PAGE_ROUNDUP(N)   (((N) + PAGE_SIZE - 1) & ~(PAGE_SIZE - 1))
#define PAGE_ROUNDDOWN(N) (((N)) & ~(PAGE_SIZE - 1))

#define TSS_ALIGNMENT 8

#endif
