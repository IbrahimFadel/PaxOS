#include "interrupts/idt.h"
#include "interrupts/irq.h"
#include "interrupts/isr.h"
#include "gdt.h"
#include "pit.h"
#include "scheduler.h"
#include "syscall/syscall.h"

static void set_idt_gate(int n, uint32_t handler, priv_t priv);

static __attribute__((aligned(IDT_ALIGNMENT))) gate_descriptor_t idt[IDT_NUM_GATES];

void idt_init(void) {
  set_idt_gate(0, (uint32_t)isr0, PRIV_KERNEL);
  set_idt_gate(1, (uint32_t)isr1, PRIV_KERNEL);
  set_idt_gate(2, (uint32_t)isr2, PRIV_KERNEL);
  set_idt_gate(3, (uint32_t)isr3, PRIV_KERNEL);
  set_idt_gate(4, (uint32_t)isr4, PRIV_KERNEL);
  set_idt_gate(5, (uint32_t)isr5, PRIV_KERNEL);
  set_idt_gate(6, (uint32_t)isr6, PRIV_KERNEL);
  set_idt_gate(7, (uint32_t)isr7, PRIV_KERNEL);
  set_idt_gate(8, (uint32_t)isr8, PRIV_KERNEL);
  set_idt_gate(9, (uint32_t)isr9, PRIV_KERNEL);
  set_idt_gate(10, (uint32_t)isr10, PRIV_KERNEL);
  set_idt_gate(11, (uint32_t)isr11, PRIV_KERNEL);
  set_idt_gate(12, (uint32_t)isr12, PRIV_KERNEL);
  set_idt_gate(13, (uint32_t)isr13, PRIV_KERNEL);
  set_idt_gate(14, (uint32_t)isr14, PRIV_KERNEL);
  set_idt_gate(15, (uint32_t)isr15, PRIV_KERNEL);
  set_idt_gate(16, (uint32_t)isr16, PRIV_KERNEL);
  set_idt_gate(17, (uint32_t)isr17, PRIV_KERNEL);
  set_idt_gate(18, (uint32_t)isr18, PRIV_KERNEL);
  set_idt_gate(19, (uint32_t)isr19, PRIV_KERNEL);
  set_idt_gate(20, (uint32_t)isr20, PRIV_KERNEL);
  set_idt_gate(21, (uint32_t)isr21, PRIV_KERNEL);
  set_idt_gate(22, (uint32_t)isr22, PRIV_KERNEL);
  set_idt_gate(23, (uint32_t)isr23, PRIV_KERNEL);
  set_idt_gate(24, (uint32_t)isr24, PRIV_KERNEL);
  set_idt_gate(25, (uint32_t)isr25, PRIV_KERNEL);
  set_idt_gate(26, (uint32_t)isr26, PRIV_KERNEL);
  set_idt_gate(27, (uint32_t)isr27, PRIV_KERNEL);
  set_idt_gate(28, (uint32_t)isr28, PRIV_KERNEL);
  set_idt_gate(29, (uint32_t)isr29, PRIV_KERNEL);
  set_idt_gate(30, (uint32_t)isr30, PRIV_KERNEL);
  set_idt_gate(31, (uint32_t)isr31, PRIV_KERNEL);
  set_idt_gate(0x80, (uint32_t)isr128, PRIV_USER);

  set_idt_gate(32 + IRQ0, (uint32_t)irq0, PRIV_KERNEL);
  set_idt_gate(32 + IRQ1, (uint32_t)irq1, PRIV_KERNEL);
  set_idt_gate(32 + IRQ2, (uint32_t)irq2, PRIV_KERNEL);
  set_idt_gate(32 + IRQ3, (uint32_t)irq3, PRIV_KERNEL);
  set_idt_gate(32 + IRQ4, (uint32_t)irq4, PRIV_KERNEL);
  set_idt_gate(32 + IRQ5, (uint32_t)irq5, PRIV_KERNEL);
  set_idt_gate(32 + IRQ6, (uint32_t)irq6, PRIV_KERNEL);
  set_idt_gate(32 + IRQ7, (uint32_t)irq7, PRIV_KERNEL);
  set_idt_gate(32 + IRQ8, (uint32_t)irq8, PRIV_KERNEL);
  set_idt_gate(32 + IRQ9, (uint32_t)irq9, PRIV_KERNEL);
  set_idt_gate(32 + IRQ10, (uint32_t)irq10, PRIV_KERNEL);
  set_idt_gate(32 + IRQ11, (uint32_t)irq11, PRIV_KERNEL);
  set_idt_gate(32 + IRQ12, (uint32_t)irq12, PRIV_KERNEL);
  set_idt_gate(32 + IRQ13, (uint32_t)irq13, PRIV_KERNEL);
  set_idt_gate(32 + IRQ14, (uint32_t)irq14, PRIV_KERNEL);
  set_idt_gate(32 + IRQ15, (uint32_t)irq15, PRIV_KERNEL);

  isrs_zero();
  register_isr(IVEC_DIV_ERR, div_err_isr);
  register_isr(IVEC_PAGE_FAULT, page_fault_isr);
  register_isr(IVEC_GENERAL_PROT, general_prot_isr);
  register_isr(IVEC_SYSCALL, syscall_handler);

  pit_init(KCONFIG_TICK_RATE_HZ);
  register_irq(TIMER_IRQ, timer_irq);
}

void idt_load(void) {
  idtr_t idtr = {.size = sizeof(idt) - 1, .offset = (uint32_t)idt};
  __asm__ __volatile__("lidt %0" : : "m"(idtr));
}

static void set_idt_gate(int n, uint32_t handler, priv_t priv) {
  idt[n] = GATE_DESCRIPTOR(handler, GDT_SELECTOR(GDT_KERNEL_CODE_IDX), GATE_TYPE_INT_32BIT, priv);
}
