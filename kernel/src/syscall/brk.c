#include "syscall/brk.h"
#include "i386/mmap_config.h"
#include "mem/pmm.h"
#include "mem/vmm.h"
#include "scheduler.h"
#include <stdint.h>
#include "utils.h"

void *sbrk(int increment) {
  //   uint32_t old_brk = current_proc->brk;
  //   uint32_t new_brk = old_brk + increment;

  //   // Map new pages if needed
  //   for (uint32_t va = ALIGN_UP(old_brk, PAGE_SIZE); va < ALIGN_UP(new_brk, PAGE_SIZE);
  //        va += PAGE_SIZE) {
  //     void *page = pmm_alloc_page();
  //     vmm_map_page(current_proc->page_dir, (void *)va, page, PTE_PRESENT | PTE_RW | PTE_USER);
  //   }

  //   current_proc->brk = new_brk;
  //   return (void *)old_brk;
  return NULL;
}
