#include "tty.h"

#include <pax/memory.h>
#include <pax/tty.h>
#include <stdarg.h>
#include <stdbool.h>
#include <string.h>

void tty_init() {
  vga_colour = vga_char_colour(VGA_COLOR_YELLOW, VGA_COLOR_BLACK);
  vga_row = VGA_HEIGHT - 1;
  vga_col = 0;
  vga_buf = (u16*)0xC03FF000;

  for (size_t y = 0; y < VGA_HEIGHT; y++) {
    for (size_t x = 0; x < VGA_WIDTH; x++) {
      const size_t idx = VGA_IDX(x, y);
      vga_buf[idx] = vga_char(' ', vga_colour);
    }
  }
}

void tty_set_colour(vga_char_colour_t colour) { vga_colour = colour; }

void tty_putchar(char c) {
  if (c == '\n') {
    tty_newline();
  } else {
    tty_putchar_raw(c, vga_colour, vga_col, vga_row);
    if (++vga_col == VGA_WIDTH) {
      tty_newline();
      vga_col = 0;
    }
  }
}

void tty_write(const char* data, size_t size) {
  for (size_t i = 0; i < size; i++) {
    const char c = data[i];
    const bool ascii = (0x20 <= c && c <= 0x7e);
    const char illegal = 0xfe;

    if (ascii || c == '\n') {
      tty_putchar(c);
    } else {
      tty_putchar(illegal);
    }
  }
}

void tty_writestring(const char* data) { tty_write(data, strlen(data)); }

uint8_t vga_char_colour(enum vga_colour_t fg, enum vga_colour_t bg) {
  return fg | bg << 4;
}

static void tty_newline() {
  for (size_t y = 1; y < VGA_HEIGHT; y++) {
    for (size_t x = 0; x < VGA_WIDTH; x++) {
      const size_t idx = VGA_IDX(x, y);
      const uint16_t vga_c = vga_buf[idx];
      const char c = vga_c;
      const vga_char_colour_t colour = vga_c >> 8;
      tty_putchar_raw(c, colour, x, y - 1);
    }
  }
  tty_clear_row(VGA_HEIGHT - 1);
  vga_col = 0;
}

static void tty_clear_row(size_t row) {
  for (size_t x = 0; x < VGA_WIDTH; x++) {
    const size_t idx = VGA_IDX(x, row);
    vga_buf[idx] = vga_char(' ', vga_colour);
  }
}

static void tty_putchar_raw(char c, vga_char_colour_t colour, size_t x,
                            size_t y) {
  const size_t idx = VGA_IDX(x, y);
  vga_buf[idx] = vga_char(c, colour);
}

static inline uint16_t vga_char(unsigned char c, uint8_t color) {
  return (uint16_t)c | (uint16_t)color << 8;
}
