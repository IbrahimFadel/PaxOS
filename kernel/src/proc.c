#include "proc.h"
#include "i386/cpu.h"
#include "i386/mmap_config.h"
#include "logging/logging.h"
#include "mem/kmalloc.h"
#include "mem/vmm.h"
#include <assert.h>
#include <string.h>

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
  stack_top -= sizeof(uint32_t);
  *(uint32_t *)stack_top = (uint32_t)entry;
  // eflags
  stack_top -= sizeof(uint32_t);
  *(uint32_t *)stack_top = EFLAGS_IF | EFLAGS_RESERVED;
  // popal pops: edi,esi,ebp,esp,ebx,edx,ecx,eax
  stack_top -= sizeof(uint32_t);
  *(uint32_t *)stack_top = 0;   // eax
  stack_top -= sizeof(uint32_t);
  *(uint32_t *)stack_top = 0;   // ecx
  stack_top -= sizeof(uint32_t);
  *(uint32_t *)stack_top = 0;   // edx
  stack_top -= sizeof(uint32_t);
  *(uint32_t *)stack_top = 0;   // ebx
  stack_top -= sizeof(uint32_t);
  *(uint32_t *)stack_top = 0;   // esp (ignored by popal)
  stack_top -= sizeof(uint32_t);
  *(uint32_t *)stack_top = 0;   // ebp
  stack_top -= sizeof(uint32_t);
  *(uint32_t *)stack_top = 0;   // esi
  stack_top -= sizeof(uint32_t);
  *(uint32_t *)stack_top = 0;   // edi
  proc->tf = (void *)stack_top; // point to edi

  proc->page_dir = vmm_va_to_pa(kernel_page_dir);

  LOGD("process_create: pid=%d entry=0x%x stack=0x%x tf=0x%x\n", proc->pid, entry,
       proc->kernel_stack, proc->tf);
  LOGD("process_create: tf=0x%x expected=0x%x sizeof_tf=%d\n", proc->tf,
       (void *)(stack_top - sizeof(trap_frame_t)), sizeof(trap_frame_t));

  return proc;
}

void process_destroy(proc_t *proc) {
  assert(proc);
  kfree(proc->kernel_stack);
  kfree(proc);
}
