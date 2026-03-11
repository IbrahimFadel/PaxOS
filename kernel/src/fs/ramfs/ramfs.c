#include "fs/ramfs/ramfs.h"
#include "i386/mmap.h"
#include "logging/logging.h"
#include <string.h>

static ramfs_file_t files[KCONFIG_RAMFS_MAX_NUM_FILES];

void ramfs_init(void) { memset(files, 0, sizeof(files)); }

fd_t ramfs_create(const char *name, uint8_t *data, uint32_t size) {
  for (int i = 0; i < KCONFIG_RAMFS_MAX_NUM_FILES; i++) {
    if (files[i].used) continue;
    strncpy(files[i].name, name, KCONFIG_RAMFS_MAX_NAME_LEN);
    files[i].name[KCONFIG_RAMFS_MAX_NAME_LEN - 1] = '\0';
    files[i].data = data;
    files[i].size = size;
    files[i].used = true;
    LOGD("ramfs_create: created %s\n", name);
    return i;
  }
  return -1;
}

ramfs_file_t *ramfs_find(const char *name) {
  for (int i = 0; i < KCONFIG_RAMFS_MAX_NUM_FILES; i++) {
    if (!files[i].used) continue;
    LOGI("%s vs %s\n", files[i].name, name);
    if (strncmp(files[i].name, name, KCONFIG_RAMFS_MAX_NAME_LEN) == 0) {
      LOGD("ramfs_find: found %s\n", name);
      return &files[i];
    }
  }
  LOGE("ramfs_find: could not find %s\n", name);
  return NULL;
}
