#include <pax/memory.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

#define CEIL_DIV(a, b) (((a + b) - 1) / b)

#define PAGE_SIZE 0x1000
#define NUM_PAGE_DIRS 256
#define NUM_PAGE_FRAMES (0x100000000 / PAGE_SIZE / 8)

// static u32 page_frame_min;
// static u32 page_frame_max;
// static u32 total_alloc;
// static u32 page_dirs[NUM_PAGE_DIRS][NUM_ENTRIES]
//     __attribute__((aligned(PAGE_SIZE)));
// static u32 page_dirs_used[NUM_PAGE_DIRS];

// u8 pmem_bitmap[NUM_PAGE_FRAMES / 8];

// static inline void invalidate_page(u32 vaddr) {
//   __asm__ volatile("invlpg %0" ::"m"(vaddr));
// }

// static void pmm_init(u32 mem_low, u32 mem_high) {
//   page_frame_min = CEIL_DIV(mem_low, PAGE_SIZE);
//   page_frame_max = CEIL_DIV(mem_high, PAGE_SIZE);
//   total_alloc = 0;
//   memset(pmem_bitmap, 0, sizeof(pmem_bitmap));
// }

// void mem_init(u32 mem_high, u32 phys_alloc_start) {
//   page_dir[0] = 0;
//   invalidate_page(0);
//   page_dir[NUM_ENTRIES - 1] = ((u32)page_dir - KERNEL_START) | 0x3;
//   invalidate_page(0xFFFFF000);

//   pmm_init(phys_alloc_start, mem_high);
//   memset(page_dirs, 0, PAGE_SIZE * NUM_PAGE_DIRS);
//   memset(page_dirs_used, 0, NUM_PAGE_DIRS);
// }

void *get_physaddr(void *virtualaddr) {
  unsigned long pdindex = (unsigned long)virtualaddr >> 22;
  unsigned long ptindex = (unsigned long)virtualaddr >> 12 & 0x03FF;

  unsigned long *pd = (unsigned long *)0xFFFFF000;  // Last PD entry
  // Here you need to check whether the PD entry is present.

  unsigned long *pt = ((unsigned long *)0xFFC00000) + (0x400 * pdindex);
  // Here you need to check whether the PT entry is present.

  return (void *)((pt[ptindex] & ~0xFFF) +
                  ((unsigned long)virtualaddr & 0xFFF));
}

void map_page(void *physaddr, void *virtualaddr, unsigned int flags) {
  // Make sure that both addresses are page-aligned.

  unsigned long pdindex = (unsigned long)virtualaddr >> 22;
  unsigned long ptindex = (unsigned long)virtualaddr >> 12 & 0x03FF;

  unsigned long *pd = (unsigned long *)0xFFFFF000;
  // Here you need to check whether the PD entry is present.
  // When it is not present, you need to create a new empty PT and
  // adjust the PDE accordingly.

  unsigned long *pt = ((unsigned long *)0xFFC00000) + (0x400 * pdindex);
  // Here you need to check whether the PT entry is present.
  // When it is, then there is already a mapping present. What do you do now?

  pt[ptindex] = ((unsigned long)physaddr) | (flags & 0xFFF) | 0x01;  // Present

  // Now you need to flush the entry in the TLB
  // or you might not notice the change.
}
