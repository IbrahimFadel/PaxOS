#include "mem/kmalloc.h"
#include "i386/mmap.h"
#include "i386/mmap_config.h"
#include "ipc/spinlock.h"
#include "logging/logging.h"
#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

typedef struct run {
  struct run *next;
} run_t;

typedef struct {
  spinlock_t lock;
  run_t *freelist;
} kmem_t;

static void free_range(void *va_start, void *va_end);

static kmem_t kmem;
static uintptr_t kernel_pa_end;

void kmem_init(void) {
  kmem.lock = spinlock_init();
  kernel_pa_end = (uintptr_t)ld_kernel_end - KERNEL_VA;
  LOGD("kmem_init: kernel_pa_end=0x%x KERNEL_PHY_END=0x%x\n", kernel_pa_end, KERNEL_PHY_END);
  assert(kernel_pa_end <= KERNEL_PHY_END);
  free_range((void *)(kernel_pa_end + KERNEL_VA), (void *)(KERNEL_PHY_END + KERNEL_VA));
  LOGD("kmem_init: freelist=0x%x\n", kmem.freelist);
}

void *kmalloc(size_t size) {
  LOGD("kmalloc: 0x%x\n", size);
  struct run *r;

  spinlock_lock(&kmem.lock);
  r = kmem.freelist;
  if (r) { kmem.freelist = r->next; }
  spinlock_unlock(&kmem.lock);

  if (r) { memset((uint8_t *)r, 0xDE, PAGE_SIZE); }
  void *p = (void *)r;
  LOGD("kmalloc: p = 0x%x\n", p);
  return p;
}

void kfree(void *va) {
  LOGD("kfree: 0x%x\n", va);
  uintptr_t pa = (uintptr_t)va - KERNEL_VA;
  assert((uintptr_t)pa % PAGE_SIZE == 0);
  assert((uintptr_t)pa >= kernel_pa_end);
  assert((uintptr_t)pa < KERNEL_PHY_END);

  memset(va, 0xBE, PAGE_SIZE);
  run_t *r = (run_t *)va;

  spinlock_lock(&kmem.lock);
  r->next = kmem.freelist;
  kmem.freelist = r;
  spinlock_unlock(&kmem.lock);
}

static void free_range(void *va_start, void *va_end) {
  LOGT("free_range: va_start = 0x%x, va_end = 0x%x\n", va_start, va_end);
  uint8_t *p = (uint8_t *)PAGE_ROUNDUP((uintptr_t)va_start);
  for (; (uintptr_t)(p + PAGE_SIZE) <= (uintptr_t)va_end; p += PAGE_SIZE) kfree((void *)p);
}
