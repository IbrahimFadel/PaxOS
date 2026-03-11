#ifndef KERNEL_FS_RAMFS_INITRD_H
#define KERNEL_FS_RAMFS_INITRD_H

#include <stdint.h>

#define USTAR_SECTOR_SIZE    512
#define USTAR_FILE_NAME_SIZE 100

typedef enum {
  USTAR_FILE = '0',
  USTAR_HARD_LINK = '1',
  USTAR_SYMLINK = '2',
  USTAR_CHAR_DEV = '3',
  USTAR_BLOCK_DEV = '4',
  USTAR_DIR = '5',
  USTAR_NAMED_PIPE = '6'
} ustar_type_t;

typedef struct {
  char name[USTAR_FILE_NAME_SIZE];
  char mode[8];
  char uid[8];
  char gid[8];
  char size_oct[12];
  char last_modification_time[12];
  char checksum[8];
  char type;
  char linked_file[USTAR_FILE_NAME_SIZE];
  char magic[6];
  char version[2];
  char owner_user_name[32];
  char owner_group_name[32];
  char device_major_number[8];
  char device_minor_number[8];
  char filename_prefix[155];
} ustar_header_t;

void initrd_load(uint32_t phys_start, uint32_t phys_end);
int oct2bin(char *str, int size);

#endif

