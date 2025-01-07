#include <pax/boot_info.h>
#include <pax/memory.h>
#include <stdio.h>

int mb_verify_boot(u32 magic, u32 mbi_addr) {
  if (magic != MB_MAGIC) {
    printf("expected magic number %d, got %d\n", MB_MAGIC, magic);
    return FAILURE;
  }

  if (mbi_addr & 7) {
    printf("Unaligned mbi: 0x%x\n", mbi_addr);
    return FAILURE;
  }

  return SUCCESS;
}

void mb_init(u32 addr) {
  struct mb_tag_t *tag;
  for (tag = (struct mb_tag_t *)(addr + 8); tag->type != MB_TAG_TYPE_END;
       tag = (struct mb_tag_t *)((u8 *)tag + ((tag->size + 7) & ~7))) {
    switch (tag->type) {
      case MB_TAG_TYPE_CMDLINE:
        printf("Command line = %s\n", ((struct mb_tag_string_t *)tag)->string);
        break;
      case MB_TAG_TYPE_BOOT_LOADER_NAME:
        printf("Boot loader name = %s\n",
               ((struct mb_tag_string_t *)tag)->string);
        break;
      case MB_TAG_TYPE_BASIC_MEMINFO:
        printf("mem_lower = %dKB, mem_upper = %dKB\n",
               ((struct mb_tag_basic_meminfo_t *)tag)->mem_lower,
               ((struct mb_tag_basic_meminfo_t *)tag)->mem_upper);
        break;
      case MB_TAG_TYPE_BOOTDEV:
        printf("Boot device %d,%d,%d\n",
               ((struct mb_tag_bootdev_t *)tag)->biosdev,
               ((struct mb_tag_bootdev_t *)tag)->slice,
               ((struct mb_tag_bootdev_t *)tag)->part);
        break;
      case MB_TAG_TYPE_MMAP: {
        struct mb_mmap_entry_t *mmap;

        printf("mmap\n");

        for (mmap = ((struct mb_tag_mmap_t *)tag)->entries;
             (u8 *)mmap < (u8 *)tag + tag->size;
             mmap = (struct mb_mmap_entry_t *)((unsigned long)mmap +
                                               ((struct mb_tag_mmap_t *)tag)
                                                   ->entry_size))
          printf(
              " base_addr = 0x%x%x,"
              " length = 0x%x%x, type = %d\n",
              (unsigned)(mmap->addr >> 32), (unsigned)(mmap->addr & 0xffffffff),
              (unsigned)(mmap->len >> 32), (unsigned)(mmap->len & 0xffffffff),
              (unsigned)mmap->type);
        break;
      }
      case MB_TAG_TYPE_FRAMEBUFFER: {
        printf("hi\n");
        u32 color;
        unsigned i;
        struct mb_tag_framebuffer_t *tagfb = (struct mb_tag_framebuffer_t *)tag;
        void *fb = (void *)(unsigned long)tagfb->common.framebuffer_addr;

        switch (tagfb->common.framebuffer_type) {
          case MULTIBOOT_FRAMEBUFFER_TYPE_INDEXED: {
            unsigned best_distance, distance;
            struct mb_color_t *palette;

            palette = tagfb->framebuffer_palette;

            color = 0;
            best_distance = 4 * 256 * 256;

            for (i = 0; i < tagfb->framebuffer_palette_num_colors; i++) {
              distance = (0xff - palette[i].blue) * (0xff - palette[i].blue) +
                         palette[i].red * palette[i].red +
                         palette[i].green * palette[i].green;
              if (distance < best_distance) {
                color = i;
                best_distance = distance;
              }
            }
            break;
          }
          case MULTIBOOT_FRAMEBUFFER_TYPE_RGB:
            color = ((1 << tagfb->framebuffer_blue_mask_size) - 1)
                    << tagfb->framebuffer_blue_field_position;
            break;
          case MULTIBOOT_FRAMEBUFFER_TYPE_EGA_TEXT:
            color = '\\' | 0x0100;
            break;
          default:
            color = 0xffffffff;
            break;
        }
        printf("FB: %d\n", fb);
        printf("Colour: %d\n", color);
        printf("%d\n", tagfb->common.framebuffer_width);
        printf("%d\n", tagfb->common.framebuffer_height);
        printf("%d\n", tagfb->common.framebuffer_bpp);
        for (i = 0; i < tagfb->common.framebuffer_width &&
                    i < tagfb->common.framebuffer_height;
             i++) {
          switch (tagfb->common.framebuffer_bpp) {
            case 8: {
              u8 *pixel = fb + tagfb->common.framebuffer_pitch * i + i;
              *pixel = color;
            } break;
            case 15:
            case 16: {
              u16 *pixel = fb + tagfb->common.framebuffer_pitch * i + 2 * i;
              *pixel = color;
            } break;
            case 24: {
              u32 *pixel = fb + tagfb->common.framebuffer_pitch * i + 3 * i;
              *pixel = (color & 0xffffff) | (*pixel & 0xff000000);
            } break;

            case 32: {
              u32 *pixel = fb + tagfb->common.framebuffer_pitch * i + 4 * i;
              *pixel = color;
            } break;
          }
        }
        break;
      }
      case MB_TAG_TYPE_ELF_SECTIONS:
        break;
      case MB_TAG_TYPE_APM:
        break;
      case MB_TAG_TYPE_ACPI_OLD:
        break;
      default:
        printf("unhandled multiboot tag type: %d\n", tag->type);
        return;
    }
  }
}
