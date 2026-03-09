#ifndef KERNEL_INTERRUPTS_ISR_H
#define KERNEL_INTERRUPTS_ISR_H

#include "interrupts/idt.h"

#define NUM_ISRS 129

void div_err_isr(trap_frame_t *tf);
void debug_exception_isr(trap_frame_t *tf);
void nmi_int_isr(trap_frame_t *tf);
void breakpoint_isr(trap_frame_t *tf);
void overflow_isr(trap_frame_t *tf);
void bound_range_exceeded_isr(trap_frame_t *tf);
void invalid_opcode_isr(trap_frame_t *tf);
void dev_not_available_isr(trap_frame_t *tf);
void double_fault_isr(trap_frame_t *tf);
void coprocessor_segment_overrun_isr(trap_frame_t *tf);
void invalid_tss_isr(trap_frame_t *tf);
void segment_not_present_isr(trap_frame_t *tf);
void stack_segment_fault_isr(trap_frame_t *tf);
void general_prot_isr(trap_frame_t *tf);
void page_fault_isr(trap_frame_t *tf);
void floating_point_error_isr(trap_frame_t *tf);
void alignment_check_isr(trap_frame_t *tf);
void machine_check_isr(trap_frame_t *tf);
void simd_floating_point_exception_isr(trap_frame_t *tf);
void virtualization_exception_isr(trap_frame_t *tf);
void control_protection_exception_isr(trap_frame_t *tf);
void default_interrupt_handler(trap_frame_t *tf);

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
extern void isr128(void);

typedef void (*isr_t)(trap_frame_t *reg);

void isrs_zero(void);
void register_isr(interrupt_vector_t num, isr_t isr);
void isr_handler(trap_frame_t *reg);

#endif
