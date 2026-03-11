#ifndef KERNEL_LIB_MULTIBOOT2_H
#define KERNEL_LIB_MULTIBOOT2_H

#include <stdint.h>

#define MULTIBOOT2_DATA_SECTION          __attribute__((used, section(".multiboot2.data")))
#define MULTIBOOT2_HEADER_MAGIC          0xE85250D6
#define MULTIBOOT2_SUCCESSFUL_BOOT_MAGIC 0x36d76289
#define MULTIBOOT2_NUM_TAGS              1
#define MULTIBOOT2_HEADER_CHECKSUM(__MULTIBOOT2_ARCH__, __HEADER_SIZE__) \
  (-(MULTIBOOT2_HEADER_MAGIC + __MULTIBOOT2_ARCH__ + __HEADER_SIZE__))

#define SHT_NULL     0 // inactive
#define SHT_PROGBITS 1 // code/data
#define SHT_SYMTAB   2 // symbol table
#define SHT_STRTAB   3 // string table
#define SHT_NOBITS   8 // .bss

typedef struct {
  uint32_t name;
  uint32_t type;
  uint32_t flags;
  uint32_t addr;
  uint32_t offset;
  uint32_t size;
  uint32_t link;
  uint32_t info;
  uint32_t addralign;
  uint32_t entsize;
} elf32_section_header_t;

typedef enum {
  MULTIBOOT2_ARCH_32BIT_PROT = 0x0,
  MULTIBOOT2_ARCH_32BIT_MIPS = 0x4,
} multiboot2_arch_e;

typedef struct {
  uint16_t type;
  uint16_t flags;
  uint32_t size;
} multiboot2_header_tag_t;

typedef struct {
  uint32_t magic;
  uint32_t architecture;
  uint32_t header_length;
  uint32_t checksum;
  multiboot2_header_tag_t tags[MULTIBOOT2_NUM_TAGS];
} multiboot2_header_t;

#define MULTIBOOT_TAG_ALIGN 8

typedef enum {
  MULTIBOOT2_TAG_END = 0,
  MULTIBOOT2_TAG_BOOT_COMMAND_LINE = 1,
  MULTIBOOT2_TAG_BOOTLOADER_NAME = 2,
  MULTIBOOT2_TAG_MODULES = 3,
  MULTIBOOT2_TAG_BASIC_MEM_INFO = 4,
  MULTIBOOT2_TAG_BIOS_BOOT_DEVICE = 5,
  MULTIBOOT2_TAG_MEM_MAP = 6,
  MULTIBOOT2_TAG_VBE_INFO = 7,
  MULTIBOOT2_TAG_FRAMEBUF_INFO = 8,
  MULTIBOOT2_TAG_ELF_SYMBOLS = 9,
  MULTIBOOT2_TAG_APM_TABLE = 10,
  MULTIBOOT2_TAG_SYS_TABLE_PTR_32BIT = 11,
  MULTIBOOT2_TAG_SYS_TABLE_PTR_64BIT = 12,
  MULTIBOOT2_TAG_SMBIOS_TABLES = 13,
  MULTIBOOT2_TAG_ACPI_OLD_RSDP = 14,
  MULTIBOOT2_TAG_ACPI_NEW_RSDP = 15,
  MULTIBOOT2_TAG_NET_INFO = 16,
  MULTIBOOT2_TAG_EFI_MEM_MAP = 17,
  MULTIBOOT2_TAG_EFI_BOOT_SERVICES_NOT_TERMINATED = 18,
  MULTIBOOT2_TAG_EFI_32BIT_IMAGE_HANDLE_PTR = 19,
  MULTIBOOT2_TAG_EFI_64BIT_IMAGE_HANDLE_PTR = 20,
  MULTIBOOT2_TAG_IMAGE_LOAD_BASE_PHYS_ADDR = 21,
} multiboot2_tag_type_t;

typedef struct {
  uint32_t mem_lower;
  uint32_t mem_upper;
} multiboot2_tag_basic_mem_info_t;

typedef struct {
  uint32_t biosdev;
  uint32_t partition;
  uint32_t sub_partition;
} multiboot2_tag_bios_boot_dev_t;

typedef struct {
  uint8_t str[0];
} multiboot2_tag_boot_command_line_t;

typedef struct {
  uint32_t mod_start;
  uint32_t mod_end;
  uint8_t cmdline[0];
} multiboot2_tag_module_t;

typedef struct {
  uint32_t num;
  uint32_t entry_size;
  uint32_t section_header_idx;
  // elf32_section_header_t sections[0];
} multiboot2_tag_elf_symbols_t;

typedef enum {
  MULTIBOOT2_MMAP_AVAILABLE = 1,
  MULTIBOOT2_MMAP_RESERVED = 2,
  MULTIBOOT2_MMAP_ACPI_RECLAIMABLE = 3,
  MULTIBOOT2_MMAP_NVS = 4,
  MULTIBOOT2_MMAP_BADRAM = 5,
} multiboot2_mmap_entry_type_t;

typedef struct {
  uint64_t base_addr;
  uint64_t length;
  uint32_t type;
  uint32_t reserved;
} multiboot2_tag_mem_map_entry_t;

typedef struct {
  uint32_t entry_size;
  uint32_t entry_version;
  multiboot2_tag_mem_map_entry_t entries[0];
} multiboot2_tag_mem_map_t;

typedef struct {
  uint8_t str[0];
} multiboot2_tag_bootloader_name_t;

typedef struct {
  uint16_t version;
  uint16_t cseg;
  uint32_t offset;
  uint16_t cseg_16;
  uint16_t dseg;
  uint16_t flags;
  uint16_t cseg_len;
  uint16_t cseg_16_len;
  uint16_t dseg_len;
} multiboot2_tag_apm_table_t;

typedef enum {
  MULTIBOOT2_FRAMEBUFFER_TYPE_INDEXED = 0,
  MULTIBOOT2_FRAMEBUFFER_TYPE_RGB = 1,
  MULTIBOOT2_FRAMEBUFFER_TYPE_EGA_TEXT = 2,
} multiboot2_framebuffer_type_t;

typedef struct {
  uint8_t red;
  uint8_t green;
  uint8_t blue;
} multiboot2_color_t;

typedef struct {
  uint64_t addr;
  uint32_t pitch;
  uint32_t width;
  uint32_t height;
  uint8_t bpp;
  uint8_t type;
  uint8_t reserved;
  union {
    struct {
      uint16_t palette_num_colors;
      multiboot2_color_t palette[0];
    };
    struct {
      uint8_t red_field_position;
      uint8_t red_mask_size;
      uint8_t green_field_position;
      uint8_t green_mask_size;
      uint8_t blue_field_position;
      uint8_t blue_mask_size;
    };
  };
} multiboot2_tag_framebuffer_info_t;

typedef struct {
  uint8_t rsdp[0];
} multiboot2_tag_acpi_old_rsdp_t;

typedef struct {
  uint32_t type;
  uint32_t size;
  union {
    multiboot2_tag_basic_mem_info_t basic_mem_info;
    multiboot2_tag_bios_boot_dev_t bios_boot_dev;
    multiboot2_tag_boot_command_line_t boot_cmd_line;
    multiboot2_tag_module_t module;
    multiboot2_tag_elf_symbols_t elf_symbols;
    multiboot2_tag_mem_map_t mem_map;
    multiboot2_tag_bootloader_name_t bootloader_name;
    multiboot2_tag_apm_table_t apm_table;
    multiboot2_tag_framebuffer_info_t framebuffer_info;
    multiboot2_tag_acpi_old_rsdp_t acpi_old_rsdp;
  };
} multiboot2_tag_t;

typedef struct {
  uint32_t total_size;
  uint32_t reserved;
} multiboot2_boot_info_t;

#define BOOTLOADER_NAME_MAX_LEN 32

typedef struct {
  char bootloader_name[BOOTLOADER_NAME_MAX_LEN];
  multiboot2_tag_mem_map_entry_t *mmap_entries;
  int num_mmap_entries;
  multiboot2_tag_module_t initrd_module;
} boot_info_t;

const char *multiboot2_mmap_type_to_str(multiboot2_mmap_entry_type_t mmap_type);
void multiboot2_info_parse(boot_info_t *boot_info, const multiboot2_boot_info_t *mbi);
void multiboot2_map(const multiboot2_boot_info_t *mbi_pa);
void multiboot2_unmap(void);

#endif
