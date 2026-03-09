#include "interrupts/isr.h"
#include "i386/cpu.h"
#include "interrupts/idt.h"
#include "logging/logging.h"
#include <assert.h>
#include <string.h>

static isr_t isrs[NUM_ISRS];

void isrs_zero(void) { memset(isrs, 0x0, sizeof(isrs)); }

void register_isr(interrupt_vector_t num, isr_t isr) {
  assert(num >= 0);
  assert(num < NUM_ISRS);
  isrs[num] = isr;
}

void isr_handler(trap_frame_t *reg) {
  LOGT("isr_handler: int=%d, err=0x%x, eip=0x%x\n", reg->int_no, reg->err_code, reg->eip);
  assert(reg->int_no >= 0);
  assert(reg->int_no < IDT_NUM_GATES);

  if (isrs[reg->int_no]) {
    isrs[reg->int_no](reg);
  } else {
    LOGE("isr_handler: no handler, halting...");
    for (;;) { hlt(); }
  }
}

void div_err_isr(trap_frame_t *reg) {
  LOGE("div_err_isr: eip = 0x%x\n", reg->eip);
  for (;;) { hlt(); }
}

void page_fault_isr(trap_frame_t *reg) {
  uint32_t cr2;
  __asm__ volatile("mov %%cr2, %0" : "=r"(cr2));
  LOGE("page_fault_isr: err_code = 0x%x, cr2 = 0x%x, eip = 0x%x\n", reg->err_code, cr2, reg->eip);

  if (reg->err_code & 1) { LOGE("error: present\n"); }
  if (reg->err_code & (1 << 1)) { LOGE("error: write\n"); }
  if (reg->err_code & (1 << 2)) { LOGE("error: user\n"); }
  if (reg->err_code & (1 << 3)) { LOGE("error: reserved write\n"); }
  if (reg->err_code & (1 << 4)) { LOGE("error: instr fetch\n"); }
  if (reg->err_code & (1 << 5)) { LOGE("error: prot key\n"); }
  if (reg->err_code & (1 << 6)) { LOGE("error: shadow stack\n"); }
  if (reg->err_code & (1 << 7)) { LOGE("error: sgx\n"); }

  for (;;) { hlt(); }
}

void general_prot_isr(trap_frame_t *reg) {
  LOGE("general_prot_isr: err_code = 0x%x, eip = 0x%x\n", reg->err_code, reg->eip);
  for (;;) { hlt(); }
}
