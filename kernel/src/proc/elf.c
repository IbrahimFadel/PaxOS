#include "proc/elf.h"
#include "i386/mmap_config.h"
#include "logging/logging.h"
#include "mem/pmm.h"
#include "mem/vmm.h"
#include "utils.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

proc_t *elf_load(void *elf_data, uint32_t size) {
  elf32_ehdr_t *ehdr = (elf32_ehdr_t *)elf_data;

  assert(ehdr->magic == ELF_MAGIC);
  assert(ehdr->machine == EM_386);
  assert(ehdr->type == ET_EXEC);
  assert(size >= ehdr->phoff + ehdr->phnum * sizeof(elf32_phdr_t));

  proc_t *proc = process_create_user(NULL);
  assert(proc);

  user_entry_frame_t *frame = (user_entry_frame_t *)((uint8_t *)proc->ctx + sizeof(proc_context_t)
                                                     + sizeof(uint32_t) // proc_enter_userspace
  );
  frame->eip = ehdr->entry;
  printf("1\n");
  uint32_t *pd_va = vmm_pa_to_va(proc->page_dir);
  printf("1\n");
  elf32_phdr_t *phdrs = (elf32_phdr_t *)((uint8_t *)elf_data + ehdr->phoff);
  printf("1\n");

  for (int i = 0; i < ehdr->phnum; i++) {
    elf32_phdr_t *ph = &phdrs[i];
    printf("1\n");
    if (ph->type != PT_LOAD) continue;
    printf("1\n");
    assert(ph->vaddr < 0xC0000000);
    assert(ph->memsz >= ph->filesz);

    uint32_t flags = PTE_PRESENT | PTE_USER;
    if (ph->flags & PF_W) flags |= PTE_RW;
    printf("6\n");

    uint32_t va_start = ph->vaddr & ~0xFFF;
    uint32_t va_end = ALIGN_UP(ph->vaddr + ph->memsz, PAGE_SIZE);
    printf("7\n");

    for (uint32_t va = va_start; va < va_end; va += PAGE_SIZE) {
      void *page_phys = pmm_alloc_page();
      void *page_va = vmm_pa_to_va(page_phys);
      memset(page_va, 0x0, PAGE_SIZE);
      printf("7\n");

      uint32_t page_off = va - ph->vaddr;
      if (va >= ph->vaddr && page_off < ph->filesz) {
        uint8_t *src = (uint8_t *)elf_data + ph->offset + page_off;
        uint32_t copy = MIN(PAGE_SIZE, ph->filesz - page_off);
        memcpy(page_va, src, copy);
        printf("8\n");
      }

      vmm_map_page(pd_va, (void *)va, page_phys, flags);
      printf("9\n");
    }

    LOGD("elf_load: segment %d va=0x%x filesz=0x%x memsz=0x%x\n", i, ph->vaddr, ph->filesz,
         ph->memsz);
  }

  LOGD("elf_load: pid=%d entry=0x%x\n", proc->pid, ehdr->entry);
  printf("10\n");
  return proc;
}
