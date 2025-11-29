#include <sys/io.h>

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
