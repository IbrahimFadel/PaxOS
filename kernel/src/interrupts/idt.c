#include "interrupts/idt.h"
#include "i386/cpu.h"
#include "interrupts/isr.h"
#include "gdt.h"
#include <stdio.h>

#define low_16(address)  (uint16_t)((address) & 0xFFFF)
#define high_16(address) (uint16_t)(((address) >> 16) & 0xFFFF)

static __attribute__((aligned(IDT_ALIGNMENT))) gate_descriptor_t idt[IDT_NUM_GATES];

__attribute__((naked, noreturn)) void generic_handler(void) { __asm__ __volatile__("cli; hlt"); }

void set_idt_gate(int n, uint32_t handler) {
  // idt[n].offset_low = low_16(handler);
  // idt[n].selector = 0x08;
  // idt[n].zero = 0;
  // idt[n].type_attr = 0x8E;
  // idt[n].offset_high = high_16(handler);
  idt[n]
    = GATE_DESCRIPTOR(handler, GDT_SELECTOR(GDT_KERNEL_CODE_IDX), GATE_TYPE_INT_32BIT, KERNEL_RING);
}

void isr_handler(const registers_t *r) {
  printf("isr_handler: INT=%d ERR=%x EIP=%x\n", r->int_no, r->err_code, r->eip);

  for (;;) {
    hlt();
  }
}

void irq_handler(const registers_t *r) {
  LOGT("irq_handler: INT=%d ERR=%x EIP=%x\n", r->int_no, r->err_code, r->eip);

  cli();
  for (;;) {
    hlt();
  }
}

void idt_init(void) {
  set_idt_gate(0, (uint32_t)isr0);
  set_idt_gate(1, (uint32_t)isr1);
  set_idt_gate(2, (uint32_t)isr2);
  set_idt_gate(3, (uint32_t)isr3);
  set_idt_gate(4, (uint32_t)isr4);
  set_idt_gate(5, (uint32_t)isr5);
  set_idt_gate(6, (uint32_t)isr6);
  set_idt_gate(7, (uint32_t)isr7);
  set_idt_gate(8, (uint32_t)isr8);
  set_idt_gate(9, (uint32_t)isr9);
  set_idt_gate(10, (uint32_t)isr10);
  set_idt_gate(11, (uint32_t)isr11);
  set_idt_gate(12, (uint32_t)isr12);
  set_idt_gate(13, (uint32_t)isr13);
  set_idt_gate(14, (uint32_t)isr14);
  set_idt_gate(15, (uint32_t)isr15);
  set_idt_gate(16, (uint32_t)isr16);
  set_idt_gate(17, (uint32_t)isr17);
  set_idt_gate(18, (uint32_t)isr18);
  set_idt_gate(19, (uint32_t)isr19);
  set_idt_gate(20, (uint32_t)isr20);
  set_idt_gate(21, (uint32_t)isr21);
  set_idt_gate(22, (uint32_t)isr22);
  set_idt_gate(23, (uint32_t)isr23);
  set_idt_gate(24, (uint32_t)isr24);
  set_idt_gate(25, (uint32_t)isr25);
  set_idt_gate(26, (uint32_t)isr26);
  set_idt_gate(27, (uint32_t)isr27);
  set_idt_gate(28, (uint32_t)isr28);
  set_idt_gate(29, (uint32_t)isr29);
  set_idt_gate(30, (uint32_t)isr30);
  set_idt_gate(31, (uint32_t)isr31);

  // for (int i = 0; i < IDT_NUM_GATES; i++) {
  //   idt[i] = GATE_DESCRIPTOR(generic_handler, GDT_SELECTOR(GDT_KERNEL_CODE_IDX),
  //                            GATE_TYPE_TRAP_32BIT, KERNEL_RING);
  // }
  // idt[IVEC_DIV_ERR] = GATE_DESCRIPTOR(div_err_isr, GDT_SELECTOR(GDT_KERNEL_CODE_IDX),
  //                                     GATE_TYPE_TRAP_32BIT, KERNEL_RING);
  // idt[IVEC_DEBUG_EXCEPTION] = GATE_DESCRIPTOR(
  //   debug_exception_isr, GDT_SELECTOR(GDT_KERNEL_CODE_IDX), GATE_TYPE_TRAP_32BIT, KERNEL_RING);
  // idt[IVEC_NMI_INT] = GATE_DESCRIPTOR(nmi_int_isr, GDT_SELECTOR(GDT_KERNEL_CODE_IDX),
  //                                     GATE_TYPE_INT_32BIT, KERNEL_RING);
  // idt[IVEC_BREAKPOINT] = GATE_DESCRIPTOR(breakpoint_isr, GDT_SELECTOR(GDT_KERNEL_CODE_IDX),
  //                                        GATE_TYPE_TRAP_32BIT, KERNEL_RING);
  // idt[IVEC_BOUND_RANGE_EXCEEDED] = GATE_DESCRIPTOR(
  //   bound_range_exceeded_isr, GDT_SELECTOR(GDT_KERNEL_CODE_IDX), GATE_TYPE_TRAP_32BIT, KERNEL_RING);
  // idt[IVEC_INVALID_OPCODE] = GATE_DESCRIPTOR(invalid_opcode_isr, GDT_SELECTOR(GDT_KERNEL_CODE_IDX),
  //                                            GATE_TYPE_TRAP_32BIT, KERNEL_RING);
  // idt[IVEC_DEV_NOT_AVAILABLE] = GATE_DESCRIPTOR(
  //   dev_not_available_isr, GDT_SELECTOR(GDT_KERNEL_CODE_IDX), GATE_TYPE_TRAP_32BIT, KERNEL_RING);
  // idt[IVEC_DOULBE_FAULT] = GATE_DESCRIPTOR(double_fault_isr, GDT_SELECTOR(GDT_KERNEL_CODE_IDX),
  //                                          GATE_TYPE_TRAP_32BIT, KERNEL_RING);
  // idt[IVEC_COPROCESSOR_SEGMENT_OVERRUN]
  //   = GATE_DESCRIPTOR(coprocessor_segment_overrun_isr, GDT_SELECTOR(GDT_KERNEL_CODE_IDX),
  //                     GATE_TYPE_TRAP_32BIT, KERNEL_RING);
  // idt[IVEC_INVALID_TSS] = GATE_DESCRIPTOR(invalid_tss_isr, GDT_SELECTOR(GDT_KERNEL_CODE_IDX),
  //                                         GATE_TYPE_TRAP_32BIT, KERNEL_RING);
  // idt[IVEC_STACK_SEGMENT_FAULT] = GATE_DESCRIPTOR(
  //   stack_segment_fault_isr, GDT_SELECTOR(GDT_KERNEL_CODE_IDX), GATE_TYPE_TRAP_32BIT, KERNEL_RING);
  // idt[IVEC_GENERAL_PROT] = GATE_DESCRIPTOR(general_prot_isr, GDT_SELECTOR(GDT_KERNEL_CODE_IDX),
  //                                          GATE_TYPE_TRAP_32BIT, KERNEL_RING);
  // idt[IVEC_PAGE_FAULT] = GATE_DESCRIPTOR(page_fault_isr, GDT_SELECTOR(GDT_KERNEL_CODE_IDX),
  //                                        GATE_TYPE_TRAP_32BIT, KERNEL_RING);
  // idt[IVEC_FLOATING_POINT_ERROR] = GATE_DESCRIPTOR(
  //   floating_point_error_isr, GDT_SELECTOR(GDT_KERNEL_CODE_IDX), GATE_TYPE_TRAP_32BIT, KERNEL_RING);
  // idt[IVEC_ALIGNMENT_CHECK] = GATE_DESCRIPTOR(
  //   alignment_check_isr, GDT_SELECTOR(GDT_KERNEL_CODE_IDX), GATE_TYPE_TRAP_32BIT, KERNEL_RING);
  // idt[IVEC_MACHINE_CHECK] = GATE_DESCRIPTOR(machine_check_isr, GDT_SELECTOR(GDT_KERNEL_CODE_IDX),
  //                                           GATE_TYPE_TRAP_32BIT, KERNEL_RING);
  // idt[IVEC_SIMD_FLOATING_POINT_EXCEPTION]
  //   = GATE_DESCRIPTOR(simd_floating_point_exception_isr, GDT_SELECTOR(GDT_KERNEL_CODE_IDX),
  //                     GATE_TYPE_TRAP_32BIT, KERNEL_RING);
  // idt[IVEC_VIRTUALIZATION_EXCEPTION]
  //   = GATE_DESCRIPTOR(virtualization_exception_isr, GDT_SELECTOR(GDT_KERNEL_CODE_IDX),
  //                     GATE_TYPE_TRAP_32BIT, KERNEL_RING);
  // idt[IVEC_CONTROL_PROTECTION_EXCEPTION]
  //   = GATE_DESCRIPTOR(control_protection_exception_isr, GDT_SELECTOR(GDT_KERNEL_CODE_IDX),
  //                     GATE_TYPE_TRAP_32BIT, KERNEL_RING);
}

void idt_load(void) {
  idtr_t idtr = {.size = sizeof(idt) - 1, .offset = (uint32_t)idt};
  __asm__ __volatile__("lidt %0" : : "m"(idtr));
}
