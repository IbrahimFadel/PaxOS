#include "proc.h"
#include "gdt.h"
#include "i386/cpu.h"
#include "i386/mmap.h"
#include "i386/mmap_config.h"
#include "logging/logging.h"
#include "mem/kmalloc.h"
#include "mem/page_table.h"
#include "mem/pmm.h"
#include "mem/vmm.h"
#include <assert.h>
#include <stdint.h>
#include <string.h>

static void copy_kernel_page_dir(page_table_t pt);
static uint32_t process_alloc_user_stack(proc_t *proc);
static void process_map_user_text(proc_t *proc);
static page_table_t process_create_page_dir(void);
__attribute__((naked)) static void proc_sti_on_entry(void);
__attribute__((naked)) static void proc_enter_userspace(void);

static cpu_t cpus[KCONFIG_MAX_NUM_CPUS];
static pid_t next_pid = 0;

cpu_t *cpu_current(void) {
  cpu_id_t id = cpu_get_local_apic_id();
  LOGT("cpu_current: id = 0x%x\n", id);
  assert(id >= 0);
  assert(id < KCONFIG_MAX_NUM_CPUS);
  return &cpus[id];
}

proc_t *process_create(void (*entry)(void), priv_t priv) {
  if (priv == PRIV_KERNEL) {
    return process_create_kernel(entry);
  } else if (priv == PRIV_USER) {
    return process_create_user(entry);
  }
  LOGE("process_create: invalid priv = %d\n", priv);
  return NULL;
}

proc_t *process_create_kernel(void (*entry)(void)) {
  proc_t *proc = kmalloc(sizeof(proc_t));
  assert(proc);
  memset(proc, 0, sizeof(proc_t));

  proc->pid = next_pid++;
  proc->state = PROC_READY;
  proc->kernel_stack = kmalloc(KERNEL_STACK_SIZE);
  assert(proc->kernel_stack);

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

proc_t *process_create_user(void (*entry)(void)) {
  proc_t *proc = kmalloc(sizeof(proc_t));
  assert(proc);
  memset(proc, 0, sizeof(proc_t));

  proc->pid = next_pid++;
  proc->state = PROC_READY;
  proc->kernel_stack = kmalloc(KERNEL_STACK_SIZE);
  assert(proc->kernel_stack);

  proc->page_dir = process_create_page_dir();
  uint32_t *page_dir_va = (uint32_t *)vmm_pa_to_va(proc->page_dir);
  uint32_t user_esp = process_alloc_user_stack(proc);

  uint32_t kstack_top = (uint32_t)proc->kernel_stack + KERNEL_STACK_SIZE;
  kstack_top &= ~(STACK_ALIGNMENT);
  kstack_top -= sizeof(user_entry_frame_t);
  user_entry_frame_t *frame = (user_entry_frame_t *)kstack_top;
  frame->esp = user_esp;
  frame->eflags = EFLAGS_IF | EFLAGS_RESERVED;
  frame->cs = GDT_SELECTOR(GDT_USER_CODE_IDX) | PRIV_USER;
  frame->ss = GDT_SELECTOR(GDT_USER_DATA_IDX) | PRIV_USER;
  frame->eip = (uint32_t)entry;
  kstack_top -= sizeof(uint32_t);
  *(uint32_t *)kstack_top = (uint32_t)proc_enter_userspace;

  kstack_top -= sizeof(proc_context_t);
  proc_context_t *ctx = (proc_context_t *)kstack_top;
  ctx->eip = (uint32_t)proc_sti_on_entry;
  ctx->ebx = 0;
  ctx->ebp = 0;
  ctx->esi = 0;
  ctx->edi = 0;
  proc->ctx = ctx;

  process_map_user_text(proc);

  return proc;
}

void process_destroy(proc_t *proc) {
  assert(proc);
  kfree(proc->kernel_stack);
  kfree(proc);
}

static void copy_kernel_page_dir(page_table_t pt) {
  uint32_t *va = vmm_pa_to_va(pt);
  memcpy(&va[768], &kernel_page_dir[768], 256 * sizeof(uint32_t));
}

static uint32_t process_alloc_user_stack(proc_t *proc) {
  uint32_t *page_dir_va = vmm_pa_to_va(proc->page_dir);
  for (int i = 0; i < USER_STACK_SIZE_PAGES; i++) {
    void *page = pmm_alloc_page();
    vmm_map_page(page_dir_va, (void *)(USER_STACK_BASE + i * PAGE_SIZE), page,
                 PTE_PRESENT | PTE_RW | PTE_USER);
  }
  return USER_STACK_BASE + USER_STACK_SIZE;
}

static void process_map_user_text(proc_t *proc) {
  uint32_t va = (uint32_t)&_user_text_start;
  uint32_t pa = (uint32_t)&_user_text_phys;
  uint32_t end = (uint32_t)&_user_text_end;
  uint32_t *page_dir_va = (uint32_t *)vmm_pa_to_va(proc->page_dir);

  for (uint32_t off = 0; off < end - va; off += PAGE_SIZE) {
    vmm_map_page(page_dir_va, (void *)(va + off), (void *)(pa + off), PTE_PRESENT | PTE_USER);
  }
}

static page_table_t process_create_page_dir(void) {
  page_table_t dir = pmm_alloc_page();
  copy_kernel_page_dir(dir);
  return dir;
}

// when processes are created for the first time, they never execute iret so we manually enable interrupts here
__attribute__((naked)) static void proc_sti_on_entry(void) {
  __asm__ volatile(
    "sti\n"
    "ret");
}

__attribute__((naked)) static void proc_enter_userspace(void) {
  __asm__ volatile(
    "mov %0, %%ax   \n"
    "mov %%ax, %%ds \n"
    "mov %%ax, %%es \n"
    "mov %%ax, %%fs \n"
    "mov %%ax, %%gs \n"
    "iret           \n" ::"i"(GDT_SELECTOR(GDT_USER_DATA_IDX) | PRIV_USER));
}
