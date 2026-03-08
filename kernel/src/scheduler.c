#include "scheduler.h"
#include "interrupts/irq.h"
#include "logging/logging.h"
#include "pic.h"
#include "proc.h"
#include <assert.h>

__attribute__((naked)) static void switch_context(proc_t **old, proc_t **new);
__attribute__((naked)) static void switch_to_first(proc_t *next);

static proc_t *current_proc = NULL;
static proc_t *run_queue = NULL;

void timer_irq(trap_frame_t *tf) {
  LOGT("timer_irq\n");
  pic_send_eoi(TIMER_IRQ);
  if (current_proc && current_proc->started) { current_proc->tf = tf; }
  schedule();
}

void scheduler_init(void) { pic_unmask_irq(TIMER_IRQ); }

void schedule(void) {
  proc_t *prev = current_proc;
  proc_t *next = scheduler_next();
  LOGT("schedule: prev = 0x%x, next = 0x%x\n", prev, next);

  if (!next || next == prev) return;

  next->state = PROC_RUNNING;
  if (prev) prev->state = PROC_READY;
  current_proc = next;

  LOGT("switching context\n");
  if (!prev) {
    LOGD("switch_to_first: next=0x%x tf=0x%x\n", next, next->tf);
    uint32_t *stack = (uint32_t *)next->tf;
    LOGD("  entry=0x%x eflags=0x%x\n", stack[0], stack[1]);
    LOGD("about to switch_to_first: cr3=0x%x esp=0x%x\n", next->page_dir, next->tf);
    current_proc->started = true;
    switch_to_first(next);
    LOGD("returned from switch_to_first\n");
  } else {
    LOGD("switch_context: prev = 0x%x, next = 0x%x\n", prev, next);
    switch_context(&prev, &next);
  }
}

void scheduler_add(proc_t *proc) {
  LOGD("scheduler_add: pid = %d\n", proc->pid);
  assert(proc->pid != 0);
  proc->state = PROC_READY;
  if (!run_queue) {
    run_queue = proc;
    proc->next = proc;
  } else {
    proc->next = run_queue->next;
    run_queue->next = proc;
  }
}

void scheduler_remove(proc_t *proc) {
  LOGD("scheduler_remove: pid = %d\n", proc->pid);
  if (!run_queue) return;
  proc_t *p = run_queue;
  do {
    if (p->next == proc) {
      p->next = proc->next;
      if (run_queue == proc) run_queue = proc->next;
      if (run_queue == proc) run_queue = NULL;
      return;
    }
    p = p->next;
  } while (p != run_queue);
}

proc_t *scheduler_next(void) {
  if (!run_queue) return NULL;
  do { run_queue = run_queue->next; } while (run_queue->state != PROC_READY);
  return run_queue;
}

__attribute__((naked)) static void switch_context(proc_t **old, proc_t **new) {
  __asm__ volatile(
    "movl 4(%%esp), %%eax\n"
    "movl 8(%%esp), %%edx\n"
    "movl (%%eax), %%eax\n"
    "movl (%%edx), %%edx\n"
    "pushfl\n"
    "pushal\n"
    "movl %%esp, %c0(%%eax)\n"
    "movl %c1(%%edx), %%ecx\n"
    "movl %%ecx, %%cr3\n"
    "movl %c0(%%edx), %%esp\n"
    "popal\n"
    "popfl\n"
    "ret\n"
    :
    : "i"(offsetof(proc_t, tf)), "i"(offsetof(proc_t, page_dir)));
}

__attribute__((naked)) static void switch_to_first(proc_t *next) {
  __asm__ volatile(
    "movl 4(%%esp), %%edx\n"
    "movl %c0(%%edx), %%ecx\n"
    "movl %%ecx, %%cr3\n"
    "movl %c1(%%edx), %%esp\n"
    "popal\n"
    "popfl\n"
    "ret\n"
    :
    : "i"(offsetof(proc_t, page_dir)), "i"(offsetof(proc_t, tf)));
}
