#ifndef KERNEL_INTERRUPTS_ISR_H
#define KERNEL_INTERRUPTS_ISR_H

#include "interrupts/idt.h"

#define NUM_ISRS 32

void div_err_isr(const trap_frame_t *r);
void debug_exception_isr(const trap_frame_t *r);
void nmi_int_isr(const trap_frame_t *r);
void breakpoint_isr(const trap_frame_t *r);
void overflow_isr(const trap_frame_t *r);
void bound_range_exceeded_isr(const trap_frame_t *r);
void invalid_opcode_isr(const trap_frame_t *r);
void dev_not_available_isr(const trap_frame_t *r);
void double_fault_isr(const trap_frame_t *r);
void coprocessor_segment_overrun_isr(const trap_frame_t *r);
void invalid_tss_isr(const trap_frame_t *r);
void segment_not_present_isr(const trap_frame_t *r);
void stack_segment_fault_isr(const trap_frame_t *r);
void general_prot_isr(const trap_frame_t *r);
void page_fault_isr(const trap_frame_t *r);
void floating_point_error_isr(const trap_frame_t *r);
void alignment_check_isr(const trap_frame_t *r);
void machine_check_isr(const trap_frame_t *r);
void simd_floating_point_exception_isr(const trap_frame_t *r);
void virtualization_exception_isr(const trap_frame_t *r);
void control_protection_exception_isr(const trap_frame_t *r);
void default_interrupt_handler(const trap_frame_t *r);

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

typedef void (*isr_t)(const trap_frame_t *reg);

void isrs_zero(void);
void register_isr(interrupt_vector_t num, isr_t isr);
void isr_handler(const trap_frame_t *reg);

#endif
