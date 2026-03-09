#include "scheduler.h"
#include "i386/cpu.h"
#include "i386/mmap_config.h"
#include "interrupts/irq.h"
#include "logging/logging.h"
#include "pic.h"
#include "proc.h"
#include "tasks/idle.h"
#include "tasks/task1.h"
#include "tasks/task2.h"
#include "tss.h"
#include <assert.h>
#include <stddef.h>
#include <stdint.h>

__attribute__((naked)) static void switch_context(proc_t *old, proc_t *new);
static proc_t *scheduler_next(void);

proc_t *current_proc = NULL;
static proc_t *run_queue = NULL;
static proc_t *idle_proc = NULL;

void timer_irq(trap_frame_t *tf) {
  LOGT("timer_irq\n");
  LOGT("task1_count = %d, task2_count = %d\n", task1_count, task2_count);
  if (current_proc) { current_proc->tf = tf; }
  pic_send_eoi(TIMER_IRQ);
  schedule();
}

void scheduler_init(void) {
  idle_proc = process_create_kernel(idle_task);
  idle_proc->state = PROC_RUNNING;
  current_proc = idle_proc;
  run_queue = idle_proc;
  idle_proc->next = idle_proc;

  pic_unmask_irq(TIMER_IRQ);
}

void schedule(void) {
  proc_t *prev = current_proc;
  proc_t *next = scheduler_next();
  LOGT("schedule: prev = 0x%x, next = 0x%x\n", prev, next);

  if (!next || next == prev) return;

  next->state = PROC_RUNNING;
  if (prev) prev->state = PROC_READY;
  current_proc = next;

  LOGD("switch_context: prev = 0x%x (pid=%d), next = 0x%x (pid=%d)\n", prev, prev->pid, next,
       next->pid);

  if (next->page_dir != prev->page_dir) {
    __asm__ volatile("mov %0, %%cr3" ::"r"(next->page_dir) : "memory");
  }
  tss_set_kernel_stack((uint32_t)(next->kernel_stack) + KERNEL_STACK_SIZE);
  switch_context(prev, next);
}

void scheduler_add(proc_t *proc) {
  LOGD("scheduler_add: pid = %d\n", proc->pid);
  assert(proc->pid != 0);
  uint32_t flags = irq_save();
  proc->state = PROC_READY;
  if (!run_queue) {
    run_queue = proc;
    proc->next = proc;
  } else {
    proc->next = run_queue->next;
    run_queue->next = proc;
  }
  irq_restore(flags);
}

void scheduler_remove(proc_t *proc) {
  LOGD("scheduler_remove: pid = %d\n", proc->pid);
  uint32_t flags = irq_save();
  if (!run_queue) { goto end; }
  proc_t *p = run_queue;
  do {
    if (p->next == proc) {
      p->next = proc->next;
      if (run_queue == proc) run_queue = proc->next;
      if (run_queue == proc) run_queue = NULL;
      goto end;
    }
    p = p->next;
  } while (p != run_queue);
end:
  irq_restore(flags);
}

static proc_t *scheduler_next(void) {
  proc_t *start = run_queue;
  do {
    run_queue = run_queue->next;
    if (run_queue != idle_proc && run_queue->state == PROC_READY) { return run_queue; }
  } while (run_queue != start);
  return idle_proc;
}

__attribute__((naked)) static void switch_context(proc_t *old, proc_t *new) {
  // eax ecx and edx already saved by caller
  __asm__ volatile(
    "push %%ebx\n"
    "push %%esi\n"
    "push %%edi\n"
    "push %%ebp\n"

    "movl (4 + 4*4)(%%esp), %%eax\n" // old
    "movl (8 + 4*4)(%%esp), %%edx\n" // new

    "mov %%esp, %c0(%%eax)\n"        // old->ctx = esp
    "mov %c0(%%edx), %%esp\n"        // esp = new->ctx

    "pop %%ebp\n"
    "pop %%edi\n"
    "pop %%esi\n"
    "pop %%ebx\n"

    "ret\n"
    :
    : "i"(offsetof(proc_t, ctx)));
}
