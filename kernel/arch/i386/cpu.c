#include "cpu.h"
#include "logging/logging.h"
#include <assert.h>

local_apic_id cpu_get_local_apic_id(void) {
#ifdef KCONFIG_ENABLE_ASSERTIONS
  cpuid_reg_t regs = cpuid(CPUID_MAX_INPUT_VAL);
  assert(regs.eax.max_input_val_supported >= CPUID_VERSION_INFO);
#endif

  regs = cpuid(CPUID_VERSION_INFO);
  local_apic_id id = regs.ebx.local_apic_id;
  LOGT("cpu_get_local_apic_id: local_apic_id = 0x%x\n", id);
  return id;
}
