#ifndef _INTERRUPT_H
#define _INTERRUPT_H

#include <sys/types.h>

#define IDT_SIZE 256

struct gate_desc_t {
  u16 offset_low;
  u16 seg_sel;
  u8 _reserved;
  u8 flags;
  u16 offset_high;
} __attribute__((packed));

struct idt_desc_t {
  u16 size;
  u32 offset;
} __attribute__((packed));

struct isr_regs_t {
  u32 cr2;
  u32 ds;
  u32 edi, esi, ebp, esp, ebx, edx, ecx, eax;
  u32 int_no, err_code;
  u32 eip, csm, eflags, useresp, ss;
};

typedef void (*irq_handler_t)(struct isr_regs_t *);

// These should not be public but asm needs access
void isr_handler(struct isr_regs_t *regs);
void irq_handler(struct isr_regs_t *regs);

inline void int_disable(void) { __asm__ volatile("cli"); }
inline void int_enable(void) { __asm__ volatile("sti"); }
void idt_init(void);

#endif
