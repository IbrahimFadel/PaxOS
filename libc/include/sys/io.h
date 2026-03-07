#ifndef KLIBC_SYS_IO_H
#define KLIBC_SYS_IO_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

  typedef uint16_t port_t;

  static const uint16_t COM1 = 0x3F8;

  void com1_init(void);

  uint8_t inb(port_t port);
  uint8_t inb_p(port_t port);
  uint16_t inw(port_t port);
  uint16_t inw_p(port_t port);
  uint32_t inl(port_t port);
  uint32_t inl_p(port_t port);

  void outb(uint8_t value, uint16_t port);
  void outb_p(uint8_t value, port_t port);
  void outw(uint16_t value, port_t port);
  void outw_p(uint16_t value, port_t port);
  void outl(uint32_t value, port_t port);
  void outl_p(uint32_t value, port_t port);

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
