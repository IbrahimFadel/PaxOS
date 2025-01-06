#ifndef _BOOT_INFO_H
#define _BOOT_INFO_H

#include <pax/multiboot.h>

int mb_verify_boot(u32 magic, u32 addr);
void mb_init(u32 addr);

#endif
