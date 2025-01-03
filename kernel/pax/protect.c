#include <pax/interrupt.h>
#include <pax/protect.h>
#include <string.h>

extern void gdt_flush(struct gdt_desc_t *gdt_ptr);
extern void tss_flush();
static void gdt_insert_gate(const u32 num, u32 base, u32 limit, u8 access,
                            u8 flags);
static void gdt_insert_tss(const u32 num, u16 ss0, u16 esp0);

static struct seg_desc_t gdt_entries[GDT_SIZE];
static struct gdt_desc_t gdt_desc;
static struct tss_entry_t tss_entry;

static void gdt_insert_gate(const u32 num, u32 base, u32 limit, u8 access,
                            u8 flags) {
  gdt_entries[num].base_low = (base & 0xFFFF);
  gdt_entries[num].base_middle = (base >> 16) && 0xFF;
  gdt_entries[num].base_high = (base >> 24) && 0xFF;
  gdt_entries[num].limit = (limit & 0xFFFF);
  gdt_entries[num].flags = (limit >> 16) & 0xF;
  gdt_entries[num].flags |= (flags & 0xF) << 4;
  gdt_entries[num].access = access;
}

static void gdt_insert_tss(const u32 num, u16 ss0, u16 esp0) {
  u32 base = (u32)&tss_entry;
  u32 limit = base + sizeof(tss_entry);

  gdt_insert_gate(num, base, limit, ACCESS_TSS, FLAGS_TSS);
  memset(&tss_entry, 0, sizeof(tss_entry));
  tss_entry.ss0 = ss0;
  tss_entry.esp0 = esp0;
  tss_entry.cs = 0x08 | PL3;
  tss_entry.ss = tss_entry.ds = tss_entry.es = tss_entry.fs = tss_entry.gs =
      0x10 | PL3;
}

void gdt_init(void) {
  gdt_desc.size = (sizeof(struct seg_desc_t) * GDT_SIZE) - 1;
  gdt_desc.offset = (u32)&gdt_entries;

  gdt_insert_gate(0, 0, 0, 0, 0);  // null
  gdt_insert_gate(1, 0, 0xFFFFFFFF, ACCESS_KERNEL_CODE, FLAGS);
  gdt_insert_gate(2, 0, 0xFFFFFFFF, ACCESS_KERNEL_DATA, FLAGS);
  gdt_insert_gate(3, 0, 0xFFFFFFFF, ACCESS_USER_CODE, FLAGS);
  gdt_insert_gate(4, 0, 0xFFFFFFFF, ACCESS_USER_DATA, FLAGS);
  gdt_insert_tss(5, 0x10, 0x0);

  int_disable();
  gdt_flush(&gdt_desc);
  tss_flush();
}
