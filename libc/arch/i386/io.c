#include <sys/io.h>

void com1_init(void) {
  outb(0x00, COM1 + 1); // Disable interrupts
  outb(0x80, COM1 + 3); // Enable DLAB (set baud rate divisor)
  outb(0x03, COM1 + 0); // Divisor low byte: 3 → 38400 baud
  outb(0x00, COM1 + 1); // Divisor high byte
  outb(0x03, COM1 + 3); // 8 bits, no parity, one stop bit (clear DLAB)
  outb(0xC7, COM1 + 2); // Enable FIFO, clear, 14-byte threshold
  outb(0x0B, COM1 + 4); // IRQs enabled, RTS/DSR set
}

void outb(u8 val, port_t port) {
  __asm__ __volatile__("outb %b0, %w1" : : "a"(val), "Nd"(port) : "memory");
}

void outw(u16 val, port_t port) {
  __asm__ __volatile__("outw %w0, %w1" : : "a"(val), "Nd"(port) : "memory");
}

u8 inb(port_t port) {
  u8 ret;
  __asm__ __volatile__("inb %w1, %b0" : "=a"(ret) : "Nd"(port) : "memory");
  return ret;
}

u16 inw(port_t port) {
  u16 ret;
  __asm__ __volatile__("inw %w1, %w0" : "=a"(ret) : "Nd"(port) : "memory");
  return ret;
}
