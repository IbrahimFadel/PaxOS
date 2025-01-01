#include <kernel/interrupt.h>
#include <kernel/tty.h>
#include <stdio.h>
#include <string.h>
#include <sys/io.h>

extern void idt_flush(u32 idt_ptr);
extern void isr0(void);
extern void isr1(void);
extern void isr2(void);
extern void isr3(void);
extern void isr4(void);
extern void isr5(void);
extern void isr6(void);
extern void isr7(void);
extern void isr8(void);
extern void isr9(void);
extern void isr10(void);
extern void isr11(void);
extern void isr12(void);
extern void isr13(void);
extern void isr14(void);
extern void isr15(void);
extern void isr16(void);
extern void isr17(void);
extern void isr18(void);
extern void isr19(void);
extern void isr20(void);
extern void isr21(void);
extern void isr22(void);
extern void isr23(void);
extern void isr24(void);
extern void isr25(void);
extern void isr26(void);
extern void isr27(void);
extern void isr28(void);
extern void isr29(void);
extern void isr30(void);
extern void isr31(void);

extern void irq0(void);
extern void irq1(void);
extern void irq2(void);
extern void irq3(void);
extern void irq4(void);
extern void irq5(void);
extern void irq6(void);
extern void irq7(void);
extern void irq8(void);
extern void irq9(void);
extern void irq10(void);
extern void irq11(void);
extern void irq12(void);
extern void irq13(void);
extern void irq14(void);
extern void irq15(void);

extern void isr128(void);
extern void isr177(void);
static void idt_insert_gate(const u32 num, u32 offset, u32 seg_sel, u8 flags);

static struct gate_desc_t idt_entries[IDT_SIZE];
static struct idt_desc_t idt;

void idt_init(void) {
  const size_t idt_size = sizeof(struct gate_desc_t) * IDT_SIZE;
  idt.size = (u16)idt_size - 1;
  idt.offset = (u32)&idt_entries;
  memset(&idt_entries, 0, idt_size);

  outb(0x11, 0x20);
  outb(0x11, 0xA0);

  outb(0x20, 0x21);
  outb(0x28, 0xA1);

  outb(0x04, 0x21);
  outb(0x02, 0xA1);

  outb(0x01, 0x21);
  outb(0x01, 0xA1);

  outb(0x0, 0x21);
  outb(0x0, 0xA1);

  idt_insert_gate(0, (u32)isr0, 0x08, 0x8E);
  idt_insert_gate(1, (u32)isr1, 0x08, 0x8E);
  idt_insert_gate(2, (u32)isr2, 0x08, 0x8E);
  idt_insert_gate(3, (u32)isr3, 0x08, 0x8E);
  idt_insert_gate(4, (u32)isr4, 0x08, 0x8E);
  idt_insert_gate(5, (u32)isr5, 0x08, 0x8E);
  idt_insert_gate(6, (u32)isr6, 0x08, 0x8E);
  idt_insert_gate(7, (u32)isr7, 0x08, 0x8E);
  idt_insert_gate(8, (u32)isr8, 0x08, 0x8E);
  idt_insert_gate(9, (u32)isr9, 0x08, 0x8E);
  idt_insert_gate(10, (u32)isr10, 0x08, 0x8E);
  idt_insert_gate(11, (u32)isr11, 0x08, 0x8E);
  idt_insert_gate(12, (u32)isr12, 0x08, 0x8E);
  idt_insert_gate(13, (u32)isr13, 0x08, 0x8E);
  idt_insert_gate(14, (u32)isr14, 0x08, 0x8E);
  idt_insert_gate(15, (u32)isr15, 0x08, 0x8E);
  idt_insert_gate(16, (u32)isr16, 0x08, 0x8E);
  idt_insert_gate(17, (u32)isr17, 0x08, 0x8E);
  idt_insert_gate(18, (u32)isr18, 0x08, 0x8E);
  idt_insert_gate(19, (u32)isr19, 0x08, 0x8E);
  idt_insert_gate(20, (u32)isr20, 0x08, 0x8E);
  idt_insert_gate(21, (u32)isr21, 0x08, 0x8E);
  idt_insert_gate(22, (u32)isr22, 0x08, 0x8E);
  idt_insert_gate(23, (u32)isr23, 0x08, 0x8E);
  idt_insert_gate(24, (u32)isr24, 0x08, 0x8E);
  idt_insert_gate(25, (u32)isr25, 0x08, 0x8E);
  idt_insert_gate(26, (u32)isr26, 0x08, 0x8E);
  idt_insert_gate(27, (u32)isr27, 0x08, 0x8E);
  idt_insert_gate(28, (u32)isr28, 0x08, 0x8E);
  idt_insert_gate(29, (u32)isr29, 0x08, 0x8E);
  idt_insert_gate(30, (u32)isr30, 0x08, 0x8E);
  idt_insert_gate(31, (u32)isr31, 0x08, 0x8E);

  idt_insert_gate(32, (u32)irq0, 0x08, 0x8E);
  idt_insert_gate(33, (u32)irq1, 0x08, 0x8E);
  idt_insert_gate(34, (u32)irq2, 0x08, 0x8E);
  idt_insert_gate(35, (u32)irq3, 0x08, 0x8E);
  idt_insert_gate(36, (u32)irq4, 0x08, 0x8E);
  idt_insert_gate(37, (u32)irq5, 0x08, 0x8E);
  idt_insert_gate(38, (u32)irq6, 0x08, 0x8E);
  idt_insert_gate(39, (u32)irq7, 0x08, 0x8E);
  idt_insert_gate(40, (u32)irq8, 0x08, 0x8E);
  idt_insert_gate(41, (u32)irq9, 0x08, 0x8E);
  idt_insert_gate(42, (u32)irq10, 0x08, 0x8E);
  idt_insert_gate(43, (u32)irq11, 0x08, 0x8E);
  idt_insert_gate(44, (u32)irq12, 0x08, 0x8E);
  idt_insert_gate(45, (u32)irq13, 0x08, 0x8E);
  idt_insert_gate(46, (u32)irq14, 0x08, 0x8E);
  idt_insert_gate(47, (u32)irq15, 0x08, 0x8E);

  idt_insert_gate(128, (u32)isr128, 0x08, 0x8E);
  idt_insert_gate(177, (u32)isr177, 0x08, 0x8E);

  idt_flush((u32)&idt);
  int_enable();
}

static void idt_insert_gate(const u32 num, u32 offset, u32 seg_sel, u8 flags) {
  idt_entries[num].offset_low = offset & 0xFFFF;
  idt_entries[num].offset_high = (offset >> 16) & 0xFFFF;
  idt_entries[num].seg_sel = seg_sel;
  idt_entries[num].flags = flags | 0x60;
}

const char *int_msgs[] = {
    "Division Error",
    "Debug",
    "Non Maskable Interrupt",
    "Breakpoint",
    "Into Detected Overflow",
    "Out of Bounds",
    "Invalid Opcode",
    "No Co-Processor",
    "Double Fault",
    "Co-Processor Segment Overrun",
    "Bad TSS",
    "Segment Not Present",
    "Stack Fault",
    "General Protection Fault",
    "Page Fault",
    "Unknown Interrupt",
    "Co-Processor Fault",
    "Alignment Fault",
    "Machine Check"
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
};

static irq_handler_t irq_handlers[] = {0, 0, 0, 0, 0, 0, 0, 0,
                                       0, 0, 0, 0, 0, 0, 0, 0};

void isr_handler(struct isr_regs_t *regs) {
  if (regs->int_no < 32) {
    tty_set_colour(vga_char_colour(VGA_COLOR_RED, VGA_COLOR_BLACK));
    tty_writestring(int_msgs[regs->int_no]);
    tty_writestring("\nException: halting\n");
  }
  __asm__ volatile("hlt" :);
}
void irq_handler(struct isr_regs_t *regs) {
  const int idx = regs->int_no - 32;
  if (idx < 16) {
    irq_handler_t handler = irq_handlers[idx];

    if (handler) {
      handler(regs);
    }

    if (regs->int_no >= 40) {
      outb(0x20, 0xA0);
    }

    outb(0x20, 0x20);
  }
}