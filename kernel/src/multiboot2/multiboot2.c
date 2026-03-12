#include "multiboot2/multiboot2.h"
#include "i386/mmap.h"
#include "mem/bootstrap.h"
#include "mem/pmm.h"
#include <assert.h>
#include <stdint.h>
#include <string.h>

MULTIBOOT2_DATA_SECTION multiboot2_header_t header
  = {.magic = MULTIBOOT2_HEADER_MAGIC,
     .architecture = MULTIBOOT2_ARCH_32BIT_PROT,
     .header_length = sizeof(header),
     .checksum = MULTIBOOT2_HEADER_CHECKSUM(MULTIBOOT2_ARCH_32BIT_PROT, sizeof(header)),
     .tags = {{0}}};

static int num_pages = 0;

void multiboot2_map(const multiboot2_boot_info_t *mbi_pa) {
  bootstrap_mem_map_page((void *)BOOTSTRAP_MAP_BASE, (void *)((uintptr_t)mbi_pa & ~0xFFF));

  const multiboot2_boot_info_t *mbi
    = (const multiboot2_boot_info_t *)(BOOTSTRAP_MAP_BASE + ((uint32_t)mbi_pa & 0xFFF));

  uintptr_t offset = (uintptr_t)mbi_pa & 0xFFF;
  uintptr_t size = offset + mbi->total_size;

  num_pages = (size + PAGE_SIZE - 1) / PAGE_SIZE;

  for (int i = 0; i < (num_pages - 1); i++) {
    uintptr_t va = BOOTSTRAP_MAP_BASE + PAGE_SIZE * (i + 1);
    uintptr_t pa = (((uintptr_t)mbi_pa & ~0xFFF) + PAGE_SIZE * (i + 1));
    bootstrap_mem_map_page((void *)va, (void *)pa);
  }
}

void multiboot2_unmap(void) {
  assert(num_pages);
  for (int i = 0; i < num_pages; i++) {
    uintptr_t va = BOOTSTRAP_MAP_BASE + PAGE_SIZE * i;
    bootstrap_mem_unmap_page((void *)va);
  }
}

void multiboot2_info_parse(boot_info_t *boot_info, const multiboot2_boot_info_t *mbi) {
  uint32_t size = mbi->total_size;
  multiboot2_tag_t *tag;
  for (tag = (multiboot2_tag_t *)((uint32_t)mbi + 8); tag->type != MULTIBOOT2_TAG_END;
       tag = (multiboot2_tag_t *)((uint8_t *)tag + ((tag->size + 7) & ~7))) {
    switch ((multiboot2_tag_type_t)tag->type) {
    case MULTIBOOT2_TAG_END:               break;
    case MULTIBOOT2_TAG_BOOT_COMMAND_LINE: break;
    case MULTIBOOT2_TAG_BOOTLOADER_NAME:
      strncpy(boot_info->bootloader_name, (const char *)tag->bootloader_name.str,
              BOOTLOADER_NAME_MAX_LEN);
      break;
    case MULTIBOOT2_TAG_MODULES:          break;
    case MULTIBOOT2_TAG_BASIC_MEM_INFO:   break;
    case MULTIBOOT2_TAG_BIOS_BOOT_DEVICE: break;
    case MULTIBOOT2_TAG_MEM_MAP:          {
      boot_info->num_mmap_entries
        = (tag->size - (4 * sizeof(uint32_t))) / sizeof(multiboot2_tag_mem_map_entry_t);
      boot_info->mmap_entries = tag->mem_map.entries;
      break;
    }
    case MULTIBOOT2_TAG_VBE_INFO:      break;
    case MULTIBOOT2_TAG_FRAMEBUF_INFO: {
      uint32_t color;
      unsigned i;
      multiboot2_tag_framebuffer_info_t *tagfb = &tag->framebuffer_info;
      void *fb = (void *)(uint32_t)tagfb->addr;

      // printf("framebuf type = %d, width = %d, height = %d\n", tagfb->type, tagfb->width,
      //        tagfb->height);

      switch (tagfb->type) {
      case MULTIBOOT2_FRAMEBUFFER_TYPE_INDEXED: {
        unsigned best_distance, distance;
        multiboot2_color_t *palette;

        palette = tagfb->palette;

        color = 0;
        best_distance = 4 * 256 * 256;

        for (i = 0; i < tagfb->palette_num_colors; i++) {
          distance = (0xff - palette[i].blue) * (0xff - palette[i].blue)
                     + palette[i].red * palette[i].red + palette[i].green * palette[i].green;
          if (distance < best_distance) {
            color = i;
            best_distance = distance;
          }
        }
      } break;

      case MULTIBOOT2_FRAMEBUFFER_TYPE_RGB:
        color = ((1 << tagfb->blue_mask_size) - 1) << tagfb->blue_field_position;
        break;

      case MULTIBOOT2_FRAMEBUFFER_TYPE_EGA_TEXT: color = '\\' | 0x0100; break;

      default:                                   color = 0xffffffff; break;
      }
      break;
    }
    case MULTIBOOT2_TAG_ELF_SYMBOLS: {
      // printf("ELF symbols:\n"
      //        " num = %d, entry size = %d, section header idx = %d\n",
      //        tag->elf_symbols.num, tag->elf_symbols.entry_size,
      //        tag->elf_symbols.section_header_idx);

      // elf32_section_header_t *sections =
      //     (elf32_section_header_t *)((uint8_t *)tag + 20);
      // elf32_section_header_t *section_header_tab =
      //     &sections[tag->elf_symbols.section_header_idx];

      // uint32_t shstrtab_pa = 0x100000 + section_header_tab->offset;

      // map_phys(shstrtab_pa & ~0xFFF, SHSTRTAB_VA);
      // char *strings = (char *)(SHSTRTAB_VA + (shstrtab_pa & 0xFFF));

      // for (uint32_t i = 0; i < tag->elf_symbols.num; i++) {
      //   elf32_section_header_t *s = &sections[i];
      //   printf("name_idx: %d  type: %d  addr: 0x%x  size: 0x%x\n", s->name,
      //          s->type, s->addr, s->size);
      // }

      break;
    }
    case MULTIBOOT2_TAG_APM_TABLE:
      // printf("APM table:\nversion = %d, cseg = %d, offset = %d\n", tag->apm_table.version,
      //  tag->apm_table.cseg, tag->apm_table.offset);
      break;
    case MULTIBOOT2_TAG_SYS_TABLE_PTR_32BIT:
    case MULTIBOOT2_TAG_SYS_TABLE_PTR_64BIT:
    case MULTIBOOT2_TAG_SMBIOS_TABLES:       break;
    case MULTIBOOT2_TAG_ACPI_OLD_RSDP:
      //  printf("ACPI old RSDP: %d\n", tag->acpi_old_rsdp);
      break;
    case MULTIBOOT2_TAG_ACPI_NEW_RSDP:
    case MULTIBOOT2_TAG_NET_INFO:
    case MULTIBOOT2_TAG_EFI_MEM_MAP:
    case MULTIBOOT2_TAG_EFI_BOOT_SERVICES_NOT_TERMINATED:
    case MULTIBOOT2_TAG_EFI_32BIT_IMAGE_HANDLE_PTR:
    case MULTIBOOT2_TAG_EFI_64BIT_IMAGE_HANDLE_PTR:
    case MULTIBOOT2_TAG_IMAGE_LOAD_BASE_PHYS_ADDR:        break;
    }
  }
  tag = (multiboot2_tag_t *)((uint8_t *)tag + ((tag->size + 7) & ~7));
  // printf("Total mbi size 0x%x\n", (uint32_t)tag - (uint32_t)mbi);
}

const char *multiboot2_mmap_type_to_str(multiboot2_mmap_entry_type_t mmap_type) {
  assert(mmap_type >= MULTIBOOT2_MMAP_AVAILABLE && mmap_type <= MULTIBOOT2_MMAP_BADRAM);
  switch (mmap_type) {
  case MULTIBOOT2_MMAP_AVAILABLE:        return "AVAILABLE";
  case MULTIBOOT2_MMAP_RESERVED:         return "RESERVED";
  case MULTIBOOT2_MMAP_ACPI_RECLAIMABLE: return "ACPI RECLAIMABLE";
  case MULTIBOOT2_MMAP_NVS:              return "NVS";
  case MULTIBOOT2_MMAP_BADRAM:           return "BADRAM";
  }
  return "UNKNOWN";
}
