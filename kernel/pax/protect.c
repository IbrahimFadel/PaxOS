#include <pax/interrupt.h>
#include <pax/protect.h>
#include <string.h>

extern void gdt_flush(struct gdt_desc_t *gdt_ptr);
extern void tss_flush();

#define GATE(_base, _limit, _access, _flags)                \
  {.base_low = (_base & 0xFFFF),                            \
   .base_middle = (_base >> 16) && 0xFF,                    \
   .base_high = (_base >> 24) && 0xFF,                      \
   .limit = (_limit & 0xFFFF),                              \
   .flags = ((_limit >> 16) & 0xF) | ((_flags & 0xF) << 4), \
   .access = _access}

static struct tss_entry_t tss_entry = {.cr3 = 0,
                                       .cs = 0x08 | PL3,
                                       .ds = 0x10 | PL3,
                                       .eax = 0,
                                       .ebp = 0,
                                       .ebx = 0,
                                       .ecx = 0,
                                       .edi = 0,
                                       .edx = 0,
                                       .eflags = 0,
                                       .eip = 0,
                                       .es = 0x10 | PL3,
                                       .esi = 0,
                                       .esp0 = 0x0,
                                       .esp1 = 0,
                                       .esp2 = 0,
                                       .esp = 0,
                                       .fs = 0x10 | PL3,
                                       .gs = 0x10 | PL3,
                                       .iopb = 0,
                                       .ldtr = 0,
                                       .prev_tss = 0,
                                       .ss0 = 0x10,
                                       .ss1 = 0,
                                       .ss2 = 0,
                                       .ss = 0x10 | PL3,
                                       .ssp = 0};
static struct seg_desc_t gdt_entries[GDT_SIZE] = {
    GATE(0, 0, 0, 0),
    GATE(0, 0xFFFFFFFF, ACCESS_KERNEL_CODE, FLAGS),
    GATE(0, 0xFFFFFFFF, ACCESS_KERNEL_DATA, FLAGS),
    GATE(0, 0xFFFFFFFF, ACCESS_USER_CODE, FLAGS),
    GATE(0, 0xFFFFFFFF, ACCESS_USER_DATA, FLAGS),
};
static struct gdt_desc_t gdt_desc = {
    .size = (sizeof(struct seg_desc_t) * GDT_SIZE) - 1,
    .offset = (u32)&gdt_entries};

void gdt_init(void) {
  const u32 base = (u32)&tss_entry;
  const u32 limit = base + sizeof(struct tss_entry_t);
  const struct seg_desc_t tss_seg = GATE(base, limit, ACCESS_TSS, FLAGS_TSS);
  gdt_entries[TSS_IDX] = tss_seg;

  int_disable();
  gdt_flush(&gdt_desc);
  tss_flush();
}
