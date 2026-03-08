#include "mem/kmalloc.h"
#include "i386/mmap.h"
#include "i386/mmap_config.h"
#include "ipc/spinlock.h"
#include "logging/logging.h"
#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#define SLAB_MAGIC     0xDEADC0DE
#define SLAB_MIN_SHIFT 5

typedef struct run {
  struct run *next;
} run_t;

typedef struct {
  spinlock_t lock;
  run_t *freelist;
} kmem_t;

typedef struct slab_hdr {
  uint32_t bin_index;
#ifdef KCONFIG_ENABLE_ASSERTIONS
  uint32_t magic;
#endif
} slab_hdr_t;

typedef struct slab_run {
  struct slab_run *next;
} slab_run_t;

typedef struct {
  spinlock_t lock;
  slab_run_t *freelist;
  size_t chunk_size;
} slab_bin_t;

static slab_bin_t bins[KCONFIG_KMALLOC_NUM_BINS];

static void free_range(void *va_start, void *va_end);
static size_t round_bin_size(size_t size);
static void *kmalloc_page(void);
static void kfree_page(void *va);
static int size_to_bin(size_t size);
static void slab_refill(slab_bin_t *bin);
static void slab_init(void);

static kmem_t kmem;
static uintptr_t kernel_pa_end;

void kmem_init(void) {
  kmem.lock = spinlock_init();
  kernel_pa_end = (uintptr_t)ld_kernel_end - KERNEL_VA;
  LOGD("kmem_init: kernel_pa_end=0x%x KERNEL_PHY_END=0x%x\n", kernel_pa_end, KERNEL_PHY_END);
  assert(kernel_pa_end <= KERNEL_PHY_END);
  free_range((void *)(kernel_pa_end + KERNEL_VA), (void *)(KERNEL_PHY_END + KERNEL_VA));
  LOGD("kmem_init: freelist=0x%x\n", kmem.freelist);
  slab_init();
}

void *kmalloc(size_t size) {
  LOGD("kmalloc: size = 0x%x\n", size);
  if (size > PAGE_SIZE / 2) { return kmalloc_page(); }

  int idx = size_to_bin(size);
  LOGT("kmalloc: bin = %d\n", idx);
  assert(idx >= 0);

  slab_bin_t *bin = &bins[idx];

  spinlock_lock(&bin->lock);
  if (!bin->freelist) slab_refill(bin);
  slab_run_t *run = bin->freelist;
  bin->freelist = run->next;
  spinlock_unlock(&bin->lock);

  slab_hdr_t *hdr = (slab_hdr_t *)run;
  hdr->bin_index = idx;
#ifdef KCONFIG_ENABLE_ASSERTIONS
  hdr->magic = SLAB_MAGIC;
#endif

  return (void *)(hdr + 1);
}

void kfree(void *ptr) {
  if (ptr == NULL) return;

  slab_hdr_t *hdr = (slab_hdr_t *)ptr - 1;
#ifdef KCONFIG_ENABLE_ASSERTIONS
  assert(hdr->magic == SLAB_MAGIC);
  hdr->magic = 0;
#endif
  assert(hdr->bin_index < KCONFIG_KMALLOC_NUM_BINS);

  slab_bin_t *bin = &bins[hdr->bin_index];
  slab_run_t *run = (slab_run_t *)hdr;

  spinlock_lock(&bin->lock);
  run->next = bin->freelist;
  bin->freelist = run;
  spinlock_unlock(&bin->lock);
}

static void slab_init(void) {
  for (int i = 0; i < KCONFIG_KMALLOC_NUM_BINS; i++) {
    bins[i].lock = spinlock_init();
    bins[i].freelist = NULL;
    bins[i].chunk_size = 1 << (SLAB_MIN_SHIFT + i);
  }
}

static void slab_refill(slab_bin_t *bin) {
  size_t total = sizeof(slab_hdr_t) + bin->chunk_size;
  size_t per_page = PAGE_SIZE / total;
  assert(per_page > 0);

  uint8_t *page = (uint8_t *)kmalloc_page();
  assert(page);

  for (size_t i = 0; i < per_page; i++) {
    slab_run_t *run = (slab_run_t *)(page + i * total);
    run->next = bin->freelist;
    bin->freelist = run;
  }
}

static int size_to_bin(size_t size) {
  if (size <= 0) return 0;
  size_t s = size - 1;
  int bit = 0;
  while (s >> (SLAB_MIN_SHIFT + bit)) bit++;
  if (bit >= KCONFIG_KMALLOC_NUM_BINS) return -1;
  return bit;
}

// PAGE LEVEL ALLOCATOR

static void *kmalloc_page(void) {
  struct run *r;

  spinlock_lock(&kmem.lock);
  r = kmem.freelist;
  if (r) { kmem.freelist = r->next; }
  spinlock_unlock(&kmem.lock);

  if (r) { memset((uint8_t *)r, 0xDE, PAGE_SIZE); }
  void *p = (void *)r;
  LOGD("kmalloc_page: p = 0x%x\n", p);
  return p;
}

static void kfree_page(void *va) {
  LOGD("kfree_page: 0x%x\n", va);
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
  for (; (uintptr_t)(p + PAGE_SIZE) <= (uintptr_t)va_end; p += PAGE_SIZE) kfree_page((void *)p);
}
