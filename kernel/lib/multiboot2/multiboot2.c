#include "multiboot2.h"

MULTIBOOT2_DATA_SECTION multiboot2_header_s header = {
    .magic = MULTIBOOT2_MAGIC,
    .architecture = MULTIBOOT2_ARCH_32BIT_PROT,
    .header_length = sizeof(header),
    .checksum =
        MULTIBOOT2_HEADER_CHECKSUM(MULTIBOOT2_ARCH_32BIT_PROT, sizeof(header)),
    .tags = {{0}}};
