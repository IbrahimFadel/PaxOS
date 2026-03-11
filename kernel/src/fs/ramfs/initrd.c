#include "fs/ramfs/initrd.h"
#include "fs/ramfs/ramfs.h"
#include "i386/mmap_config.h"
#include "mem/kmalloc.h"
#include "logging/logging.h"
#include "mem/vmm.h"
#include "utils.h"
#include <stdint.h>
#include <string.h>

void initrd_load(uint32_t phys_start, uint32_t phys_end) {
  LOGD("initrd_load: start = 0x%x, end = 0x%x\n", phys_start, phys_end);
  uint8_t *ptr = vmm_pa_to_va((void *)phys_start);
  uint8_t *end = vmm_pa_to_va((void *)phys_end);

  while (ptr < end) {
    ustar_header_t *hdr = (ustar_header_t *)ptr;
    if (hdr->name[0] == '\0') break;

    uint32_t size = oct2bin(hdr->size_oct, 12);
    if (hdr->type == USTAR_FILE || hdr->type == '\0') {
      uint8_t *data = kmalloc(size);
      memcpy(data, ptr + USTAR_SECTOR_SIZE, size);
      ramfs_create(hdr->name, data, size);
      LOGD("initrd: loaded '%s' (%d bytes)\n", hdr->name, size);
    }

    ptr += USTAR_SECTOR_SIZE + ALIGN_UP(size, USTAR_SECTOR_SIZE);
  }
}

int oct2bin(char *str, int size) {
  int n = 0;
  char *c = str;
  while (size-- > 0) {
    n *= 8;
    n += *c - '0';
    c++;
  }
  return n;
}
