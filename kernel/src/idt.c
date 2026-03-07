#include "idt.h"
#include "gdt.h"
#include "i386/cpu.h"
#include <stdio.h>

static __attribute__((aligned(IDT_ALIGNMENT))) gate_descriptor_t idt[IDT_NUM_GATES];

void div_err_isr(void) {
  printf("fault: division by 0\n");
  cli();
  hlt();
}
void debug_exception_isr(void) {}
void nmi_int_isr(void) {}
void breakpoint_isr(void) {}
void overflow_isr(void) {}
void bound_range_exceeded_isr(void) {}
void invalid_opcode_isr(void) {}
void dev_not_available_isr(void) {}
void double_fault_isr(void) {}
void coprocessor_segment_overrun_isr(void) {}
void invalid_tss_isr(void) {}
void segment_not_present_isr(void) {}
void stack_segment_fault_isr(void) {}
void general_prot_isr(void) {}
void page_fault_isr(void) {
  cli();
  printf("page fault\n");
  hlt();
}
void floating_point_error_isr(void) {}
void alignment_check_isr(void) {}
void machine_check_isr(void) {}
void simd_floating_point_exception_isr(void) {}
void virtualization_exception_isr(void) {}
void control_protection_exception_isr(void) {}

__attribute__((naked, noreturn)) void generic_handler(void) { __asm__ __volatile__("cli; hlt"); }

void idt_init(void) {
  for (int i = 0; i < IDT_NUM_GATES; i++) {
    idt[i] = GATE_DESCRIPTOR(generic_handler, GDT_SELECTOR(GDT_KERNEL_CODE_IDX),
                             GATE_TYPE_TRAP_32BIT, KERNEL_RING);
  }
  idt[IVEC_DIV_ERR] = GATE_DESCRIPTOR(div_err_isr, GDT_SELECTOR(GDT_KERNEL_CODE_IDX),
                                      GATE_TYPE_TRAP_32BIT, KERNEL_RING);
  idt[IVEC_DEBUG_EXCEPTION] = GATE_DESCRIPTOR(
    debug_exception_isr, GDT_SELECTOR(GDT_KERNEL_CODE_IDX), GATE_TYPE_TRAP_32BIT, KERNEL_RING);
  idt[IVEC_NMI_INT] = GATE_DESCRIPTOR(nmi_int_isr, GDT_SELECTOR(GDT_KERNEL_CODE_IDX),
                                      GATE_TYPE_INT_32BIT, KERNEL_RING);
  idt[IVEC_BREAKPOINT] = GATE_DESCRIPTOR(breakpoint_isr, GDT_SELECTOR(GDT_KERNEL_CODE_IDX),
                                         GATE_TYPE_TRAP_32BIT, KERNEL_RING);
  idt[IVEC_BOUND_RANGE_EXCEEDED] = GATE_DESCRIPTOR(
    bound_range_exceeded_isr, GDT_SELECTOR(GDT_KERNEL_CODE_IDX), GATE_TYPE_TRAP_32BIT, KERNEL_RING);
  idt[IVEC_INVALID_OPCODE] = GATE_DESCRIPTOR(invalid_opcode_isr, GDT_SELECTOR(GDT_KERNEL_CODE_IDX),
                                             GATE_TYPE_TRAP_32BIT, KERNEL_RING);
  idt[IVEC_DEV_NOT_AVAILABLE] = GATE_DESCRIPTOR(
    dev_not_available_isr, GDT_SELECTOR(GDT_KERNEL_CODE_IDX), GATE_TYPE_TRAP_32BIT, KERNEL_RING);
  idt[IVEC_DOULBE_FAULT] = GATE_DESCRIPTOR(double_fault_isr, GDT_SELECTOR(GDT_KERNEL_CODE_IDX),
                                           GATE_TYPE_TRAP_32BIT, KERNEL_RING);
  idt[IVEC_COPROCESSOR_SEGMENT_OVERRUN]
    = GATE_DESCRIPTOR(coprocessor_segment_overrun_isr, GDT_SELECTOR(GDT_KERNEL_CODE_IDX),
                      GATE_TYPE_TRAP_32BIT, KERNEL_RING);
  idt[IVEC_INVALID_TSS] = GATE_DESCRIPTOR(invalid_tss_isr, GDT_SELECTOR(GDT_KERNEL_CODE_IDX),
                                          GATE_TYPE_TRAP_32BIT, KERNEL_RING);
  idt[IVEC_STACK_SEGMENT_FAULT] = GATE_DESCRIPTOR(
    stack_segment_fault_isr, GDT_SELECTOR(GDT_KERNEL_CODE_IDX), GATE_TYPE_TRAP_32BIT, KERNEL_RING);
  idt[IVEC_GENERAL_PROT] = GATE_DESCRIPTOR(general_prot_isr, GDT_SELECTOR(GDT_KERNEL_CODE_IDX),
                                           GATE_TYPE_TRAP_32BIT, KERNEL_RING);
  idt[IVEC_PAGE_FAULT] = GATE_DESCRIPTOR(page_fault_isr, GDT_SELECTOR(GDT_KERNEL_CODE_IDX),
                                         GATE_TYPE_TRAP_32BIT, KERNEL_RING);
  idt[IVEC_FLOATING_POINT_ERROR] = GATE_DESCRIPTOR(
    floating_point_error_isr, GDT_SELECTOR(GDT_KERNEL_CODE_IDX), GATE_TYPE_TRAP_32BIT, KERNEL_RING);
  idt[IVEC_ALIGNMENT_CHECK] = GATE_DESCRIPTOR(
    alignment_check_isr, GDT_SELECTOR(GDT_KERNEL_CODE_IDX), GATE_TYPE_TRAP_32BIT, KERNEL_RING);
  idt[IVEC_MACHINE_CHECK] = GATE_DESCRIPTOR(machine_check_isr, GDT_SELECTOR(GDT_KERNEL_CODE_IDX),
                                            GATE_TYPE_TRAP_32BIT, KERNEL_RING);
  idt[IVEC_SIMD_FLOATING_POINT_EXCEPTION]
    = GATE_DESCRIPTOR(simd_floating_point_exception_isr, GDT_SELECTOR(GDT_KERNEL_CODE_IDX),
                      GATE_TYPE_TRAP_32BIT, KERNEL_RING);
  idt[IVEC_VIRTUALIZATION_EXCEPTION]
    = GATE_DESCRIPTOR(virtualization_exception_isr, GDT_SELECTOR(GDT_KERNEL_CODE_IDX),
                      GATE_TYPE_TRAP_32BIT, KERNEL_RING);
  idt[IVEC_CONTROL_PROTECTION_EXCEPTION]
    = GATE_DESCRIPTOR(control_protection_exception_isr, GDT_SELECTOR(GDT_KERNEL_CODE_IDX),
                      GATE_TYPE_TRAP_32BIT, KERNEL_RING);
}

void idt_load(void) {
  idtr_t idtr = {.size = sizeof(idt) - 1, .offset = (uint32_t)idt};
  __asm__ __volatile__("lidt %0" : : "m"(idtr));
}
