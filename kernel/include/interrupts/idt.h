#ifndef KERNEL_INTERRUPTS_IDT_H
#define KERNEL_INTERRUPTS_IDT_H

#include <stdint.h>

#define IDT_ALIGNMENT        16
#define IDT_NUM_GATES        256
#define IDT_NUM_USER_DEFINED 224

#define GATE_DESCRIPTOR(OFFSET, SELECTOR, TYPE, PRIV)                                             \
  (((gate_descriptor_t)(uint32_t)(OFFSET) & 0xFFFF)                                               \
   | (((gate_descriptor_t)(SELECTOR) & 0xFFFF) << 16) | (((gate_descriptor_t)(TYPE) & 0xF) << 40) \
   | (((gate_descriptor_t)(PRIV) & 0x3) << 45) | ((gate_descriptor_t)1 << 47)                     \
   | (((gate_descriptor_t)(uint32_t)(OFFSET) & 0xFFFF0000) << 32))

typedef enum {
  GATE_TYPE_TASK = 0x5,
  GATE_TYPE_INT_16BIT = 0x6,
  GATE_TYPE_TRAP_16BIT = 0x7,
  GATE_TYPE_INT_32BIT = 0xE,
  GATE_TYPE_TRAP_32BIT = 0xF
} gate_type_t;

typedef uint64_t gate_descriptor_t;

typedef enum {
  IVEC_DIV_ERR = 0,
  IVEC_DEBUG_EXCEPTION = 1,
  IVEC_NMI_INT = 2,
  IVEC_BREAKPOINT = 3,
  IVEC_OVERFLOW = 4,
  IVEC_BOUND_RANGE_EXCEEDED = 5,
  IVEC_INVALID_OPCODE = 6,
  IVEC_DEV_NOT_AVAILABLE = 7,
  IVEC_DOULBE_FAULT = 8,
  IVEC_COPROCESSOR_SEGMENT_OVERRUN = 9,
  IVEC_INVALID_TSS = 10,
  IVEC_SEGMENT_NOT_PRESENT = 11,
  IVEC_STACK_SEGMENT_FAULT = 12,
  IVEC_GENERAL_PROT = 13,
  IVEC_PAGE_FAULT = 14,
  IVEC_FLOATING_POINT_ERROR = 16,
  IVEC_ALIGNMENT_CHECK = 17,
  IVEC_MACHINE_CHECK = 18,
  IVEC_SIMD_FLOATING_POINT_EXCEPTION = 19,
  IVEC_VIRTUALIZATION_EXCEPTION = 20,
  IVEC_CONTROL_PROTECTION_EXCEPTION = 21,
} interrupt_vector_t;

typedef struct __attribute__((packed)) {
  uint16_t size;
  uint32_t offset;
} idtr_t;

typedef struct {
  uint32_t cr2;
  uint32_t ds;
  uint32_t edi, esi, ebp, esp, ebx, edx, ecx, eax;
  uint32_t int_no, err_code;
  uint32_t eip, csm, eflags, useresp, ss;
} registers_t;

void idt_init(void);
void idt_load(void);

#endif
