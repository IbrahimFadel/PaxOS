#include "i386/mmap.h"
#include "multiboot2/multiboot2.h"
#include "stdlib.h"
#include <pax/tty.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sys/io.h>

#define MBI_VA 0xD0000000
#define SHSTRTAB_VA 0xD0002000

void map_phys(uint32_t pa, uint32_t va);

typedef struct {
  uint32_t total_size;
  uint32_t reserved;
} multiboot2_info;

void kmain(uint32_t mb2_magic, uint32_t mb2_info_pa) {
  com1_init();
  tty_init();
  tty_writestring("hello, world\n");

  if (mb2_magic != MULTIBOOT2_SUCCESSFUL_BOOT_MAGIC) {
    tty_writestring("multiboot2 magic number incorrect\n");
    abort();
  }
  if (mb2_info_pa & 0x7) {
    tty_writestring("multiboot2 info not aligned\n");
    abort();
  }

  map_phys(mb2_info_pa & ~0xFFF, MBI_VA);
  map_phys((mb2_info_pa & ~0xFFF) + PAGE_SIZE, MBI_VA + PAGE_SIZE);
  uint32_t mbi_addr = (MBI_VA + (mb2_info_pa & 0xFFF));
  multiboot2_info *mbi = (multiboot2_info *)mbi_addr;

  uint32_t size = mbi->total_size;
  multiboot2_tag_t *tag;
  printf("mbi size 0x%x\n", size);
  for (tag = (multiboot2_tag_t *)(mbi_addr + 8);
       tag->type != MULTIBOOT2_TAG_END;
       tag = (multiboot2_tag_t *)((uint8_t *)tag + ((tag->size + 7) & ~7))) {
    printf("tag 0x%x, size 0x%x\n", tag->type, tag->size);
    switch ((multiboot2_tag_type_t)tag->type) {
    case MULTIBOOT2_TAG_END:
      break;
    case MULTIBOOT2_TAG_BOOT_COMMAND_LINE:
      printf("boot command line = \"%s\"\n", tag->boot_cmd_line.str);
      break;
    case MULTIBOOT2_TAG_BOOTLOADER_NAME:
      printf("bootloader name = \"%s\"\n", tag->bootloader_name.str);
      break;
    case MULTIBOOT2_TAG_MODULES:
    case MULTIBOOT2_TAG_BASIC_MEM_INFO:
    case MULTIBOOT2_TAG_BIOS_BOOT_DEVICE:
      break;
    case MULTIBOOT2_TAG_MEM_MAP: {
      printf("memmap:\nentry size = %d, entry version = %d\n",
             tag->mem_map.entry_size, tag->mem_map.entry_version);
      multiboot2_tag_mem_map_entry_t *mmap;
      for (mmap = tag->mem_map.entries;
           (uint8_t *)mmap < (uint8_t *)tag + tag->size;
           mmap = (multiboot2_tag_mem_map_entry_t *)((unsigned long)mmap +
                                                     tag->mem_map.entry_size)) {
        printf(" base_addr = 0x%x%x,"
               " length = 0x%x%x, type = %s\n",
               (unsigned)(mmap->base_addr >> 32),
               (unsigned)(mmap->base_addr & 0xffffffff),
               (unsigned)(mmap->length >> 32),
               (unsigned)(mmap->length & 0xffffffff),
               multiboot2_mmap_type_to_str(mmap->type));
      }
      break;
    }
    case MULTIBOOT2_TAG_VBE_INFO:
    case MULTIBOOT2_TAG_FRAMEBUF_INFO:
      break;
    case MULTIBOOT2_TAG_ELF_SYMBOLS: {
      printf("ELF symbols:\n"
             " num = %d, entry size = %d, section header idx = %d\n",
             tag->elf_symbols.num, tag->elf_symbols.entry_size,
             tag->elf_symbols.section_header_idx);

      elf32_section_header_t *sections =
          (elf32_section_header_t *)((uint8_t *)tag + 20);
      elf32_section_header_t *section_header_tab =
          &sections[tag->elf_symbols.section_header_idx];

      uint32_t shstrtab_pa = 0x100000 + section_header_tab->offset;

      map_phys(shstrtab_pa & ~0xFFF, SHSTRTAB_VA);
      char *strings = (char *)(SHSTRTAB_VA + (shstrtab_pa & 0xFFF));

      for (uint32_t i = 0; i < tag->elf_symbols.num; i++) {
        elf32_section_header_t *s = &sections[i];
        printf("name_idx: %d  type: %d  addr: 0x%x  size: 0x%x\n", s->name,
               s->type, s->addr, s->size);
      }

      break;
    }
    case MULTIBOOT2_TAG_APM_TABLE:
      printf("APM table:\nversion = %d, cseg = %d, offset = %d\n",
             tag->apm_table.version, tag->apm_table.cseg,
             tag->apm_table.offset);
      break;
    case MULTIBOOT2_TAG_SYS_TABLE_PTR_32BIT:
    case MULTIBOOT2_TAG_SYS_TABLE_PTR_64BIT:
    case MULTIBOOT2_TAG_SMBIOS_TABLES:
    case MULTIBOOT2_TAG_ACPI_OLD_RSDP:
    case MULTIBOOT2_TAG_ACPI_NEW_RSDP:
    case MULTIBOOT2_TAG_NET_INFO:
    case MULTIBOOT2_TAG_EFI_MEM_MAP:
    case MULTIBOOT2_TAG_EFI_BOOT_SERVICES_NOT_TERMINATED:
    case MULTIBOOT2_TAG_EFI_32BIT_IMAGE_HANDLE_PTR:
    case MULTIBOOT2_TAG_EFI_64BIT_IMAGE_HANDLE_PTR:
    case MULTIBOOT2_TAG_IMAGE_LOAD_BASE_PHYS_ADDR:
      break;
    }
  }

  for (;;) {
  }
}

void map_phys(uint32_t pa, uint32_t va) {
  uint32_t pd_idx = va >> 22;
  uint32_t pt_idx = (va >> 12) & 0x3FF;

  // access PD itself via recursive mapping (PD[1023] -> PD)
  uint32_t *pd = (uint32_t *)0xFFFFF000;

  if (!(pd[pd_idx] & PAGE_PRESENT)) {
    // allocate a page for the new page table
    // for now, use a static buffer since you don't have a heap yet
    static uint8_t pt_storage[PAGE_TABLE_SIZE * PTE_SIZE]
        __attribute__((aligned(PAGE_SIZE)));
    uint32_t pt_pa = (uint32_t)pt_storage - KERNEL_VA;

    memset(pt_storage, 0, sizeof(pt_storage));
    pd[pd_idx] = pt_pa | PAGE_PRESENT | PAGE_RW;
  }

  uint32_t *pt = (uint32_t *)(0xFFC00000 + pd_idx * PAGE_SIZE);
  pt[pt_idx] = (pa & ~0xFFF) | PAGE_PRESENT | PAGE_RW;

  asm volatile("invlpg %0" ::"m"(*(char *)va) : "memory");
}

// uint32_t size = mbi->total_size;
//   multiboot2_tag_t *tag;
//   printf("mbi size 0x%x\n", size);
//   for (tag = (multiboot2_tag_t *)(mbi_addr + 8);
//        tag->type != MULTIBOOT2_TAG_END;
//        tag = (multiboot2_tag_t *)((uint8_t *)tag + ((tag->size + 7) & ~7))) {
//     printf("tag 0x%x, size 0x%x\n", tag->type, tag->size);
//     // switch ((multiboot2_tag_type_t)tag->type) {
//     // case MULTIBOOT2_TAG_END:
//     //   break;
//     // case MULTIBOOT2_TAG_BOOT_COMMAND_LINE:
//     //   // printf("boot command line = \"%s\"\n", tag->boot_cmd_line.str);
//     //   break;
//     // case MULTIBOOT2_TAG_BOOTLOADER_NAME:
//     //   // printf("bootloader name = \"%s\"\n", tag->bootloader_name.str);
//     //   break;
//     // case MULTIBOOT2_TAG_MODULES:
//     // case MULTIBOOT2_TAG_BASIC_MEM_INFO:
//     // case MULTIBOOT2_TAG_BIOS_BOOT_DEVICE:
//     //   break;
//     // case MULTIBOOT2_TAG_MEM_MAP: {
//     //   // printf("memmap:\nentry size = %d, entry version = %d\n",
//     //   //        tag->mem_map.entry_size, tag->mem_map.entry_version);

//     //   // multiboot2_tag_mem_map_entry_t *mmap;

//     //   // for (mmap = tag->mem_map.entries;
//     //   //      (uint8_t *)mmap < (uint8_t *)tag + tag->size;
//     //   //      mmap = (multiboot2_tag_mem_map_entry_t *)((unsigned
//     long)mmap +
//     //   // tag->mem_map.entry_size))
//     //   // printf(" base_addr = 0x%x%x,"
//     //   //        " length = 0x%x%x, type = %s\n",
//     //   //        (unsigned)(mmap->base_addr >> 32),
//     //   //        (unsigned)(mmap->base_addr & 0xffffffff),
//     //   //        (unsigned)(mmap->length >> 32),
//     //   //        (unsigned)(mmap->length & 0xffffffff),
//     //   //        multiboot2_mmap_type_to_str(mmap->type));
//     //   break;
//     // }
//     // case MULTIBOOT2_TAG_VBE_INFO:
//     // case MULTIBOOT2_TAG_FRAMEBUF_INFO:
//     // case MULTIBOOT2_TAG_ELF_SYMBOLS:
//     //   break;
//     // case MULTIBOOT2_TAG_APM_TABLE:
//     //   // printf("APM table:\nversion = %d, cseg = %d, offset = %d\n",
//     //   //        tag->apm_table.version, tag->apm_table.cseg,
//     //   //        tag->apm_table.offset);
//     //   break;
//     // case MULTIBOOT2_TAG_SYS_TABLE_PTR_32BIT:
//     // case MULTIBOOT2_TAG_SYS_TABLE_PTR_64BIT:
//     // case MULTIBOOT2_TAG_SMBIOS_TABLES:
//     // case MULTIBOOT2_TAG_ACPI_OLD_RSDP:
//     // case MULTIBOOT2_TAG_ACPI_NEW_RSDP:
//     // case MULTIBOOT2_TAG_NET_INFO:
//     // case MULTIBOOT2_TAG_EFI_MEM_MAP:
//     // case MULTIBOOT2_TAG_EFI_BOOT_SERVICES_NOT_TERMINATED:
//     // case MULTIBOOT2_TAG_EFI_32BIT_IMAGE_HANDLE_PTR:
//     // case MULTIBOOT2_TAG_EFI_64BIT_IMAGE_HANDLE_PTR:
//     // case MULTIBOOT2_TAG_IMAGE_LOAD_BASE_PHYS_ADDR:
//     //   break;
//     // }
//   }