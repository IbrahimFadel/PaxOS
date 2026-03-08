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

void isr_handler(const trap_frame_t *reg) {
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

void div_err_isr(const trap_frame_t *reg) {
  LOGE("div_err_isr: eip = 0x%x\n", reg->eip);
  for (;;) { hlt(); }
}

void page_fault_isr(const trap_frame_t *reg) {
  LOGE("page_fault_isr: err_code = 0x%x, eip = 0x%x\n", reg->err_code, reg->eip);
  for (;;) { hlt(); }
}
