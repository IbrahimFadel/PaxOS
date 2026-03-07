#include <sys/io.h>

void com1_init(void) {
  outb(COM1 + 1, 0x00); // Disable interrupts
  outb(COM1 + 3, 0x80); // Enable DLAB (set baud rate divisor)
  outb(COM1 + 0, 0x03); // Divisor low byte: 3 → 38400 baud
  outb(COM1 + 1, 0x00); // Divisor high byte
  outb(COM1 + 3, 0x03); // 8 bits, no parity, one stop bit (clear DLAB)
  outb(COM1 + 2, 0xC7); // Enable FIFO, clear, 14-byte threshold
  outb(COM1 + 4, 0x0B); // IRQs enabled, RTS/DSR set
}
