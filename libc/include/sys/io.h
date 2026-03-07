#ifndef KLIBC_SYS_IO_H
#define KLIBC_SYS_IO_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

  typedef uint16_t port_t;

  static const uint16_t COM1 = 0x3F8;

  void com1_init(void);

  uint8_t inb_p(port_t port);
  uint16_t inw_p(port_t port);
  uint32_t inl(port_t port);
  uint32_t inl_p(port_t port);

  void outb_p(port_t port, uint8_t value);
  void outw_p(port_t port, uint16_t value);
  void outl(port_t port, uint32_t value);
  void outl_p(port_t port, uint32_t value);

  static inline void outb(port_t port, uint8_t value) {
    __asm__ volatile("outb %b0, %w1" : : "a"(value), "Nd"(port));
  }

  static inline void outw(port_t port, uint16_t value) {
    __asm__ volatile("outw %w0, %w1" : : "a"(value), "Nd"(port));
  }

  static inline uint8_t inb(port_t port) {
    uint8_t ret;
    __asm__ volatile("inb %w1, %b0" : "=a"(ret) : "Nd"(port));
    return ret;
  }

  static inline uint16_t inw(port_t port) {
    uint16_t ret;
    __asm__ volatile("inw %w1, %w0" : "=a"(ret) : "Nd"(port));
    return ret;
  }

  static inline void io_wait(void) { outb(0x80, 0); }

#ifdef __cplusplus
}
#endif

// void insb(port_t port, void addr[.count], unsigned long count);
// void insw(port_t port, void addr[.count], unsigned long count);
// void insl(port_t port, void addr[.count], unsigned long count);
// void outsb(port_t port, const void addr[.count], unsigned long count);
// void outsw(port_t port, const void addr[.count], unsigned long count);
// void outsl(port_t port, const void addr[.count], unsigned long count);

#endif
