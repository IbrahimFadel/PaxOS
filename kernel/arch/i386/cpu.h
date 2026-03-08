#ifndef KERNEL_I386_CPU_H
#define KERNEL_I386_CPU_H

#include "logging/logging.h"
#include "stdbool.h"
#include <stdint.h>

static inline void cli(void) {
  LOGT("cli\n");
  __asm__ volatile("cli" :::);
}

static inline void sti(void) {
  LOGT("sti\n");
  __asm__ volatile("sti" :::);
}

static inline void hlt(void) {
  LOGT("hlt\n");
  __asm__ volatile("hlt" :::);
}

static inline void invlpg(void *p) {
  LOGT("invpg: 0x%x\n", p);
  __asm__ volatile("invlpg (%0)" ::"r"(p) : "memory");
}

typedef uint8_t local_apic_id;

typedef enum {
  CPUID_MAX_INPUT_VAL = 0x0,
  CPUID_VERSION_INFO = 0x1,
  CPUID_CACHE_TLB = 0x2,
  CPUID_CACHE_TYPE = 0x4,
  CPUID_MONITOR_SIZE = 0x5,
} cpuid_cmd_t;

typedef struct {
  union {
    uint32_t raw;
    uint32_t max_input_val_supported;
    struct {
      uint32_t stepping_id       : 4;
      uint32_t model             : 4;
      uint32_t family_id         : 4;
      uint32_t processor_type    : 2;
      uint32_t reserved0         : 2;
      uint32_t extended_model_id : 4;
      uint32_t extended_family_id: 8;
      uint32_t reserved1         : 4;
    };
  };
} cpuid_eax_t;

typedef struct {
  union {
    uint32_t raw;
    struct {
      uint32_t brand_index                    : 8;
      uint32_t cflush_line_size               : 8;
      uint32_t logical_processors_per_physical: 8;
      uint32_t local_apic_id                  : 8;
    };
  };
} cpuid_ebx_t;

typedef struct {
  union {
    uint32_t raw;
    struct {
      uint32_t SSE3     : 1;
      uint32_t reserved0: 2;
      uint32_t MONITOR  : 1;
      uint32_t DS_CPL   : 1;
      uint32_t reserved1: 2;
      uint32_t EST      : 1;
      uint32_t TM2      : 1;
      uint32_t reserved2: 1;
      uint32_t CNXT_ID  : 1;
      uint32_t reserved3: 21;
    };
  };
} cpuid_ecx_t;

typedef struct {
  union {
    uint32_t raw;
    struct {
      uint32_t FPU      : 1;
      uint32_t VME      : 1;
      uint32_t DE       : 1;
      uint32_t PSE      : 1;
      uint32_t TSC      : 1;
      uint32_t MSR      : 1;
      uint32_t PAE      : 1;
      uint32_t MCE      : 1;
      uint32_t CX8      : 1;
      uint32_t APIC     : 1;
      uint32_t reserved4: 1;
      uint32_t SEP      : 1;
      uint32_t PGE      : 1;
      uint32_t MCA      : 1;
      uint32_t CMOV     : 1;
      uint32_t PAT      : 1;
      uint32_t PSE36    : 1;
      uint32_t PSN      : 1;
      uint32_t CFLUSH   : 1;
      uint32_t DS       : 1;
      uint32_t ACPI     : 1;
      uint32_t MMX      : 1;
      uint32_t FXSR     : 1;
      uint32_t SSE      : 1;
      uint32_t SSE2     : 1;
      uint32_t SS       : 1;
      uint32_t HTT      : 1;
      uint32_t TM       : 1;
      uint32_t reserved5: 1;
      uint32_t PBE      : 1;
    };
  };
} cpuid_edx_t;

typedef struct {
  cpuid_eax_t eax;
  cpuid_ebx_t ebx;
  cpuid_ecx_t ecx;
  cpuid_edx_t edx;
} cpuid_reg_t;

static inline cpuid_reg_t cpuid(uint32_t eax) {
  LOGT("cpuid: in: eax = 0x%x\n", eax);
  cpuid_reg_t regs;
  __asm__ volatile("cpuid\n"
                   : "=a"(regs.eax.raw), "=b"(regs.ebx.raw), "=c"(regs.ecx.raw), "=d"(regs.edx.raw)
                   : "a"(eax));
  LOGT("cpuid: out: eax = 0x%x, ebx = 0x%x, ecx = 0x%x, edx = 0x%x\n", regs.eax, regs.ebx, regs.ecx,
       regs.edx);
  return regs;
}

local_apic_id cpu_get_local_apic_id(void);

#define EFLAGS_RESERVED 0x2
#define EFLAGS_IF       0x200

static inline bool intr_enabled(void) {
  uint32_t eflags;
  __asm__ volatile(
    "pushfl\n"
    "pop %0\n"
    : "=r"(eflags));
  return eflags & EFLAGS_IF;
}

#endif
