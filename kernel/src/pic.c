#include "pic.h"
#include "sys/io.h"

#define MASTER_PIC_CMD  0x20
#define MASTER_PIC_DATA 0x21

#define SLAVE_PIC_CMD  0xA0
#define SLAVE_PIC_DATA 0xA1

#define ICW1_ICW4      0x01
#define ICW1_SINGLE    0x02
#define ICW1_INTERVAL4 0x04
#define ICW1_LEVEL     0x08
#define ICW1_INIT      0x10

#define ICW4_8086       0x01
#define ICW4_AUTO       0x02
#define ICW4_BUF_SLAVE  0x08
#define ICW4_BUF_MASTER 0x0C
#define ICW4_SFNM       0x10

#define SLAVE_PIC_ID    2
#define PIC_CASCADE_IRQ (1 << SLAVE_PIC_ID)

#define PIC_READ_IRR 0xA
#define PIC_READ_ISR 0xB

static uint16_t pic_get_irq_reg(int ocw3);
static void wait(void);

void pic_remap(int offset1, int offset2) {
  outb(MASTER_PIC_CMD, ICW1_INIT | ICW1_ICW4);
  wait();
  outb(SLAVE_PIC_CMD, ICW1_INIT | ICW1_ICW4);
  wait();
  outb(MASTER_PIC_DATA, offset1);
  wait();
  outb(SLAVE_PIC_DATA, offset2);
  wait();
  outb(MASTER_PIC_DATA, PIC_CASCADE_IRQ);
  wait();
  outb(SLAVE_PIC_DATA, SLAVE_PIC_ID);
  wait();
  outb(MASTER_PIC_DATA, ICW4_8086);
  wait();
  outb(SLAVE_PIC_DATA, ICW4_8086);
  wait();
}

void pic_mask_irq(uint8_t irq) {
  uint16_t port;
  uint8_t value;
  if (irq < 8) {
    port = MASTER_PIC_DATA;
  } else {
    port = SLAVE_PIC_DATA;
    irq -= 8;
  }
  value = inb(port) | (1 << irq);
  outb(port, value);
}

void pic_unmask_irq(uint8_t irq) {
  uint16_t port;
  uint8_t value;
  if (irq < 8) {
    port = MASTER_PIC_DATA;
  } else {
    port = SLAVE_PIC_DATA;
    irq -= 8;
  }
  value = inb(port) & ~(1 << irq);
  outb(port, value);
}

void pic_unmask_all(void) {
  outb(MASTER_PIC_DATA, 0);
  outb(SLAVE_PIC_DATA, 0);
}

void pic_mask_all(void) {
  outb(MASTER_PIC_DATA, 0xFF);
  outb(SLAVE_PIC_DATA, 0xFF);
}

uint16_t pic_get_irr(void) { return pic_get_irq_reg(PIC_READ_IRR); }

uint16_t pic_get_isr(void) { return pic_get_irq_reg(PIC_READ_ISR); }

static uint16_t pic_get_irq_reg(int ocw3) {
  outb(MASTER_PIC_CMD, ocw3);
  outb(SLAVE_PIC_CMD, ocw3);
  return (inb(SLAVE_PIC_CMD) << 8) | inb(MASTER_PIC_CMD);
}

static void wait(void) {
  volatile int i = 50000;
  while (i--) {}
}
