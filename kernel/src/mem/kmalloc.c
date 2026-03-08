#include "mem/kmalloc.h"
#include "i386/mmap.h"
#include "i386/mmap_config.h"
#include "ipc/spinlock.h"
#include "logging/logging.h"
#include "mem/pmm.h"
#include "mem/vmm.h"
#include "utils.h"
#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#define SLAB_MAGIC     0xDEADC0DE
#define SLAB_MIN_SHIFT 5
#define SLAB_BIN_LARGE 0xFF

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
  uint32_t num_pages;
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

static size_t round_bin_size(size_t size);
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
  slab_init();
}

void *kmalloc(size_t size) {
  LOGD("kmalloc: size = 0x%x\n", size);
  if (size > (1 << (SLAB_MIN_SHIFT + KCONFIG_KMALLOC_NUM_BINS - 1))) {
    size_t num_pages = ALIGN_UP(sizeof(slab_hdr_t) + size, PAGE_SIZE) / PAGE_SIZE;

    void *va_base = vmm_alloc_pages(num_pages);
    assert(va_base);

    slab_hdr_t *hdr = (slab_hdr_t *)va_base;
    hdr->bin_index = SLAB_BIN_LARGE;
#ifdef KCONFIG_ENABLE_ASSERTIONS
    hdr->magic = SLAB_MAGIC;
#endif
    hdr->num_pages = num_pages;
    return (void *)(hdr + 1);
  }

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

  if (hdr->bin_index == SLAB_BIN_LARGE) {
    void *va = (void *)ALIGN_DOWN((uintptr_t)hdr, PAGE_SIZE);
    vmm_free_pages(va, hdr->num_pages);
    return;
  }

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

  uint8_t *page = (uint8_t *)pmm_alloc_page();
  assert(page);
  page = vmm_pa_to_va(page);

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
