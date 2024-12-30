#ifndef VGA_H
#define VGA_H

#include <kernel/tty.h>
#include <stddef.h>
#include <stdint.h>

static const size_t VGA_WIDTH = 80;
static const size_t VGA_HEIGHT = 25;

#define VGA_IDX(x, y) (y * VGA_WIDTH + x)

static uint8_t vga_colour;
static size_t vga_row;
static uint8_t vga_col;
static volatile uint16_t* vga_buf;

static inline uint16_t vga_char(unsigned char c, uint8_t color);

static void tty_newline();
static void tty_clear_row(size_t row);
static void tty_putchar_raw(char c, vga_char_colour_t colour, size_t x,
                            size_t y);

#endif
