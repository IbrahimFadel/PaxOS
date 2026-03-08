#include "proc.h"
#include "i386/cpu.h"
#include "logging/logging.h"
#include <assert.h>

static cpu_t cpus[KCONFIG_MAX_NUM_CPUS];

cpu_t *cpu_current(void) {
  cpu_id_t id = cpu_get_local_apic_id();
  LOGT("cpu_current: id = 0x%x\n", id);
  assert(id >= 0);
  assert(id < KCONFIG_MAX_NUM_CPUS);
  return &cpus[id];
}
