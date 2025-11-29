#include "tty/tty.h"

#include <stdbool.h>
#include <stdint.h>
#include <string.h>

static inline uint16_t vga_char(unsigned char c, uint8_t color);

static void tty_newline(void);
static void tty_clear_row(size_t row);
static void tty_putchar_raw(char c, vga_char_colour_t colour, size_t x,
                            size_t y);

#define VGA_IDX(x, y) (y * VGA_WIDTH + x)

static const size_t VGA_WIDTH = 80;
static const size_t VGA_HEIGHT = 25;

static uint8_t vga_colour;
static size_t vga_row;
static uint8_t vga_col;
volatile uint16_t *TTY_BUF_BASE_ADDR = (volatile uint16_t *)0xC03FF000;

void tty_init(void) {
  vga_colour = vga_char_colour(VGA_COLOR_YELLOW, VGA_COLOR_BLACK);
  vga_row = VGA_HEIGHT - 1;
  vga_col = 0;

  for (size_t y = 0; y < VGA_HEIGHT; y++) {
    for (size_t x = 0; x < VGA_WIDTH; x++) {
      const size_t idx = VGA_IDX(x, y);
      TTY_BUF_BASE_ADDR[idx] = vga_char(' ', vga_colour);
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

void tty_write(const char *data, size_t size) {
  for (size_t i = 0; i < size; i++) {
    char c = data[i];
    bool ascii = (0x20 <= c && c <= 0x7e);

    if (ascii || c == '\n') {
      tty_putchar(c);
    } else {
      tty_putchar(TTY_ILLEGAL_CHAR);
    }
  }
}

void tty_writestring(const char *data) { tty_write(data, strlen(data)); }

uint8_t vga_char_colour(vga_colour_e fg, vga_colour_e bg) {
  return fg | bg << 4;
}

static void tty_newline(void) {
  for (size_t y = 1; y < VGA_HEIGHT; y++) {
    for (size_t x = 0; x < VGA_WIDTH; x++) {
      const size_t idx = VGA_IDX(x, y);
      const uint16_t vga_c = TTY_BUF_BASE_ADDR[idx];
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
    TTY_BUF_BASE_ADDR[idx] = vga_char(' ', vga_colour);
  }
}

static void tty_putchar_raw(char c, vga_char_colour_t colour, size_t x,
                            size_t y) {
  const size_t idx = VGA_IDX(x, y);
  TTY_BUF_BASE_ADDR[idx] = vga_char(c, colour);
}

static inline uint16_t vga_char(unsigned char c, uint8_t color) {
  return (uint16_t)c | (uint16_t)color << 8;
}
