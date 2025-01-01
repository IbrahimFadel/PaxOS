#include <kernel/protect.h>
#include <stdio.h>

extern void gdt_flush(struct gdt_desc_t *gdt_ptr);
static void gdt_insert_gate(const u32 num, u32 base, u32 limit, u8 access,
                            u8 flags);

static struct seg_desc_t gdt_entries[5];
static struct gdt_desc_t gdt_desc;

static inline void int_disable(void) { __asm__ volatile("cli"); }
static inline void int_enable(void) { __asm__ volatile("sti"); }

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

void gdt_init(void) {
  gdt_desc.size = (sizeof(struct seg_desc_t) * 5) - 1;
  gdt_desc.offset = (u32)&gdt_entries;

  gdt_insert_gate(0, 0, 0, 0, 0);  // null
  gdt_insert_gate(1, 0, 0xFFFFFFFF, ACCESS_KERNEL_CODE, FLAGS);
  gdt_insert_gate(2, 0, 0xFFFFFFFF, ACCESS_KERNEL_DATA, FLAGS);
  gdt_insert_gate(3, 0, 0xFFFFFFFF, ACCESS_USER_CODE, FLAGS);
  gdt_insert_gate(4, 0, 0xFFFFFFFF, ACCESS_USER_DATA, FLAGS);

  int_disable();
  printf("interrupts disabled\n");
  gdt_flush(&gdt_desc);
  printf("GDT setup OK\n");
  int_enable();
  printf("interrupts enabled\n");
}
