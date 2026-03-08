#include "proc.h"
#include "i386/cpu.h"
#include "i386/mmap_config.h"
#include "logging/logging.h"
#include "mem/kmalloc.h"
#include "mem/vmm.h"
#include <assert.h>
#include <stdint.h>
#include <string.h>

__attribute__((naked)) static void proc_sti_on_entry(void);

static cpu_t cpus[KCONFIG_MAX_NUM_CPUS];
static pid_t next_pid = 1;

cpu_t *cpu_current(void) {
  cpu_id_t id = cpu_get_local_apic_id();
  LOGT("cpu_current: id = 0x%x\n", id);
  assert(id >= 0);
  assert(id < KCONFIG_MAX_NUM_CPUS);
  return &cpus[id];
}

proc_t *process_create(void (*entry)(void)) {
  proc_t *proc = kmalloc(sizeof(proc_t));
  assert(proc);
  memset(proc, 0, sizeof(proc_t));

  proc->pid = next_pid++;
  proc->state = PROC_READY;
  proc->kernel_stack = kmalloc(KERNEL_STACK_SIZE);
  assert(proc->kernel_stack != NULL);

  uint32_t stack_top = (uint32_t)proc->kernel_stack + KERNEL_STACK_SIZE;
  stack_top &= ~(STACK_ALIGNMENT);

  stack_top -= sizeof(uint32_t);
  *(uint32_t *)stack_top = (uint32_t)entry;

  stack_top -= sizeof(proc_context_t);
  proc_context_t *ctx = (proc_context_t *)stack_top;
  ctx->eip = (uint32_t)proc_sti_on_entry;
  ctx->ebx = 0;
  ctx->ebp = 0;
  ctx->esi = 0;
  ctx->edi = 0;
  proc->ctx = ctx;
  proc->page_dir = vmm_va_to_pa(kernel_page_dir);

  LOGD("process_create: pid=%d entry=0x%x stack=0x%x ctx=0x%x\n", proc->pid, entry,
       proc->kernel_stack, proc->ctx);
  return proc;
}

void process_destroy(proc_t *proc) {
  assert(proc);
  kfree(proc->kernel_stack);
  kfree(proc);
}

// when processes are created for the first time, they never execute iret so we manually enable interrupts here
__attribute__((naked)) static void proc_sti_on_entry(void) {
  __asm__ volatile(
    "sti\n"
    "ret");
}
