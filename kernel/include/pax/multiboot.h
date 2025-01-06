#ifndef _MULTIBOOT_H
#define _MULTIBOOT_H

#include <sys/types.h>

// These are lifted directly from the multiboot spec

#define MB_MAGIC 0x36d76289

#define MB_TAG_TYPE_END 0
#define MB_TAG_TYPE_CMDLINE 1
#define MB_TAG_TYPE_BOOT_LOADER_NAME 2
#define MB_TAG_TYPE_MODULE 3
#define MB_TAG_TYPE_BASIC_MEMINFO 4
#define MB_TAG_TYPE_BOOTDEV 5
#define MB_TAG_TYPE_MMAP 6
#define MB_TAG_TYPE_VBE 7
#define MB_TAG_TYPE_FRAMEBUFFER 8
#define MB_TAG_TYPE_ELF_SECTIONS 9
#define MB_TAG_TYPE_APM 10
#define MB_TAG_TYPE_EFI32 11
#define MB_TAG_TYPE_EFI64 12
#define MB_TAG_TYPE_SMBIOS 13
#define MB_TAG_TYPE_ACPI_OLD 14
#define MB_TAG_TYPE_ACPI_NEW 15
#define MB_TAG_TYPE_NETWORK 16
#define MB_TAG_TYPE_EFI_MMAP 17
#define MB_TAG_TYPE_EFI_BS 18
#define MB_TAG_TYPE_EFI32_IH 19
#define MB_TAG_TYPE_EFI64_IH 20
#define MB_TAG_TYPE_LOAD_BASE_ADDR 21

struct mb_tag_t {
  u32 type;
  u32 size;
};

struct mb_tag_string_t {
  u32 type;
  u32 size;
  char string[0];
};

struct mb_tag_module_t {
  u32 type;
  u32 size;
  u32 mod_start;
  u32 mod_end;
  char cmdline[0];
};

struct mb_tag_basic_meminfo_t {
  u32 type;
  u32 size;
  u32 mem_lower;
  u32 mem_upper;
};

struct mb_tag_bootdev_t {
  u32 type;
  u32 size;
  u32 biosdev;
  u32 slice;
  u32 part;
};

struct mb_mmap_entry_t {
  u64 addr;
  u64 len;
#define MB_MEMORY_AVAILABLE 1
#define MB_MEMORY_RESERVED 2
#define MB_MEMORY_ACPI_RECLAIMABLE 3
#define MB_MEMORY_NVS 4
#define MB_MEMORY_BADRAM 5
  u32 type;
  u32 zero;
};

struct mb_tag_mmap_t {
  u32 type;
  u32 size;
  u32 entry_size;
  u32 entry_version;
  struct mb_mmap_entry_t entries[0];
};

struct mb_color_t {
  u8 red;
  u8 green;
  u8 blue;
};

struct mb_tag_framebuffer_common_t {
  u32 type;
  u32 size;

  u64 framebuffer_addr;
  u32 framebuffer_pitch;
  u32 framebuffer_width;
  u32 framebuffer_height;
  u8 framebuffer_bpp;
#define MULTIBOOT_FRAMEBUFFER_TYPE_INDEXED 0
#define MULTIBOOT_FRAMEBUFFER_TYPE_RGB 1
#define MULTIBOOT_FRAMEBUFFER_TYPE_EGA_TEXT 2
  u8 framebuffer_type;
  u16 reserved;
};

struct mb_tag_framebuffer_t {
  struct mb_tag_framebuffer_common_t common;

  union {
    struct {
      u16 framebuffer_palette_num_colors;
      struct mb_color_t framebuffer_palette[0];
    };
    struct {
      u8 framebuffer_red_field_position;
      u8 framebuffer_red_mask_size;
      u8 framebuffer_green_field_position;
      u8 framebuffer_green_mask_size;
      u8 framebuffer_blue_field_position;
      u8 framebuffer_blue_mask_size;
    };
  };
};

#endif
