#ifndef KERNEL_PROC_ELF_H
#define KERNEL_PROC_ELF_H

#include "proc.h"
#include <stdint.h>

#define ELF_MAGIC 0x464C457F
#define ET_EXEC   2
#define EM_386    3
#define PT_LOAD   1
#define PF_W      2

typedef struct __attribute__((packed)) {
  uint32_t magic;
  uint8_t class;
  uint8_t data;
  uint8_t version;
  uint8_t _pad[9];
  uint16_t type;
  uint16_t machine;
  uint32_t version2;
  uint32_t entry;
  uint32_t phoff;
  uint32_t shoff;
  uint32_t flags;
  uint16_t ehsize;
  uint16_t phentsize;
  uint16_t phnum;
  uint16_t shentsize;
  uint16_t shnum;
  uint16_t shstrndx;
} elf32_ehdr_t;

typedef struct __attribute__((packed)) {
  uint32_t type;
  uint32_t offset;
  uint32_t vaddr;
  uint32_t paddr;
  uint32_t filesz;
  uint32_t memsz;
  uint32_t flags;
  uint32_t align;
} elf32_phdr_t;

proc_t *elf_load(void *elf_data, uint32_t size);

#endif
