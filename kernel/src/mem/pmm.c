#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include "mem/pmm.h"
#include "i386/mmap.h"
#include "intrinsics.h"
#include "logging/logging.h"
#include "mem/bootstrap.h"
#include "multiboot2/multiboot2.h"

#define BITS_PER_WORD    32
#define NUM_BITMAP_BYTES (4096 * 32)
#define NUM_BITMAP_WORDS (NUM_BITMAP_BYTES / sizeof(uint32_t))

static int first_zero_bit(uint32_t word);
static void page_set_used(uint32_t page);
static void page_set_free(uint32_t page);
static int page_used(uint32_t page);
static int page_free(uint32_t page);

static uint32_t page_bitmap[NUM_BITMAP_WORDS];
static uint32_t last_word = 0;

extern char ld_kernel_end;

void pmm_init(const boot_info_t *boot_info) {
  memset(page_bitmap, 0xFFFFFFFF, sizeof(page_bitmap));

  assert(boot_info->num_mmap_entries > 0);
  for (int i = 0; i < boot_info->num_mmap_entries; i++) {
    multiboot2_tag_mem_map_entry_t *entry = &boot_info->mmap_entries[i];
    if (entry->type != MULTIBOOT2_MMAP_AVAILABLE) continue;

    uint32_t start = entry->base_addr;
    uint32_t end = start + entry->length;
    for (uint32_t addr = start; addr < end; addr += PAGE_SIZE) {
      page_set_free(addr / PAGE_SIZE);
    }
  }

  uint32_t kernel_start = (uint32_t)KERNEL_START;
  uint32_t kernel_end = (uint32_t)&ld_kernel_end;
  for (uint64_t addr = kernel_start; addr < kernel_end; addr += PAGE_SIZE) {
    page_set_used(addr / PAGE_SIZE);
  }

  page_set_used(0);
}

void *pmm_alloc_page(void) {
  for (int j = 0; j < NUM_BITMAP_WORDS; j++) {
    int i = (last_word + j) % NUM_BITMAP_WORDS;
    if (page_bitmap[i] == 0xFFFFFFFF) continue;

    int free_page = first_zero_bit(page_bitmap[i]);
    page_bitmap[i] |= 1U << free_page;
    last_word = i;
    void *addr = (void *)((i * BITS_PER_WORD + free_page) * PAGE_SIZE);
    LOGT("pmm: alloc 0x%x\n", addr);
    return addr;
  }
  LOGT("pmm: alloc failed, no free pages\n");
  return NULL;
}

void pmm_free(void *p) {
  LOGT("pmm: free 0x%x\n", p);
  assert(p);
  assert(ALIGNED((uintptr_t)p, PAGE_SIZE));
  uint32_t page = (uintptr_t)p / PAGE_SIZE;
  assert(page_used(page));
  page_set_free(page);
  uint32_t word = page / BITS_PER_WORD;
  if (word < last_word) last_word = word;
}

static int first_zero_bit(uint32_t word) {
  uint32_t inv = ~word;
  assert(inv != 0);
  return builtin_ctz(inv);
}

static void page_set_used(uint32_t page) {
  int idx = page / BITS_PER_WORD;
  assert(idx >= 0);
  assert(idx < NUM_BITMAP_WORDS);
  page_bitmap[idx] |= (1U << (page % BITS_PER_WORD));
}

static int page_used(uint32_t page) {
  int idx = page / BITS_PER_WORD;
  assert(idx >= 0);
  assert(idx < NUM_BITMAP_WORDS);
  return page_bitmap[idx] & (1U << (page % BITS_PER_WORD));
}

static void page_set_free(uint32_t page) {
  int idx = page / BITS_PER_WORD;
  assert(idx >= 0);
  assert(idx < NUM_BITMAP_WORDS);
  page_bitmap[idx] &= ~(1U << (page % BITS_PER_WORD));
}

static int page_free(uint32_t page) {
  int idx = page / BITS_PER_WORD;
  assert(idx >= 0);
  assert(idx < NUM_BITMAP_WORDS);
  return page_bitmap[idx] & ~(1U << (page % BITS_PER_WORD));
}
