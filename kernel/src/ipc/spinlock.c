#include "ipc/spinlock.h"
#include "i386/cpu.h"
#include "logging/logging.h"
#include "proc.h"
#include "stdbool.h"
#include <assert.h>
#include <stdatomic.h>

static bool spinlock_holding(const spinlock_t *lock);
static void spinlock_push_off(void);
static void spinlock_pop_off(void);
static bool spinlock_locked(const spinlock_t *lock);

spinlock_t spinlock_init(void) {
  spinlock_t lock = {.locked = 0, .cpu_id = -1};
  return lock;
}

void spinlock_lock(spinlock_t *lock) {
  spinlock_push_off();
  assert(!spinlock_holding(lock));
  while (atomic_exchange_explicit(&lock->locked, 1, memory_order_acquire));
  lock->cpu_id = cpu_get_local_apic_id();
}

void spinlock_unlock(spinlock_t *lock) {
  assert(spinlock_holding(lock));
  lock->cpu_id = 0;
  atomic_store_explicit(&lock->locked, 0, memory_order_release);
  spinlock_pop_off();
}

static bool spinlock_holding(const spinlock_t *lock) {
  return spinlock_locked(lock) && (cpu_get_local_apic_id() == lock->cpu_id);
}

static void spinlock_push_off(void) {
  int intr = intr_enabled();
  cli();
  cpu_t *cpu = cpu_current();
  if (cpu->num_interrupt_offs == 0) { cpu->interrupts_enabled = intr; }
  cpu->num_interrupt_offs++;
  LOGT("spinlock_push_off: num_interrupt_offs = %d, interrupts_enabled = %d\n",
       cpu->num_interrupt_offs, cpu->interrupts_enabled);
}

static void spinlock_pop_off(void) {
  cpu_t *cpu = cpu_current();
  assert(!intr_enabled());
  assert(cpu->num_interrupt_offs >= 1);
  cpu->num_interrupt_offs--;
  LOGT("spinlock_pop_off: num_interrupt_offs = %d, interrupts_enabled = %d\n",
       cpu->num_interrupt_offs, cpu->interrupts_enabled);
  if (cpu->num_interrupt_offs == 0 && cpu->interrupts_enabled) { sti(); }
}

static bool spinlock_locked(const spinlock_t *lock) {
  return atomic_load_explicit(&lock->locked, memory_order_relaxed);
}
