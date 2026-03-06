#include "multiboot2.h"

MULTIBOOT2_DATA_SECTION multiboot2_header_t header = {
    .magic = MULTIBOOT2_HEADER_MAGIC,
    .architecture = MULTIBOOT2_ARCH_32BIT_PROT,
    .header_length = sizeof(header),
    .checksum =
        MULTIBOOT2_HEADER_CHECKSUM(MULTIBOOT2_ARCH_32BIT_PROT, sizeof(header)),
    .tags = {{0}}};

const char *
multiboot2_mmap_type_to_str(multiboot2_mmap_entry_type_t mmap_type) {
  //   assert(mmap_type >= MULTIBOOT2_MMAP_AVAILABLE &&
  //  mmap_type <= MULTIBOOT2_MMAP_BADRAM);
  switch (mmap_type) {
  case MULTIBOOT2_MMAP_AVAILABLE:
    return "AVAILABLE";
  case MULTIBOOT2_MMAP_RESERVED:
    return "RESERVED";
  case MULTIBOOT2_MMAP_ACPI_RECLAIMABLE:
    return "ACPI RECLAIMABLE";
  case MULTIBOOT2_MMAP_NVS:
    return "NVS";
  case MULTIBOOT2_MMAP_BADRAM:
    return "BADRAM";
  }
  return "UNKNOWN";
}