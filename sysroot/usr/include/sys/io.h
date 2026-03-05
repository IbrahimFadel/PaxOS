#ifndef _IO_H
#define _IO_H

#include <sys/types.h>

typedef u16 port_t;

#ifdef __cplusplus
extern "C" {
#endif

u8 inb(port_t port);
u8 inb_p(port_t port);
u16 inw(port_t port);
u16 inw_p(port_t port);
u32 inl(port_t port);
u32 inl_p(port_t port);

void outb(uint8_t value, uint16_t port);
void outb_p(u8 value, port_t port);
void outw(u16 value, port_t port);
void outw_p(u16 value, port_t port);
void outl(u32 value, port_t port);
void outl_p(u32 value, port_t port);

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
