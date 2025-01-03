#ifndef _KERNEL_TTY_H
#define _KERNEL_TTY_H

#include <stddef.h>
#include <stdint.h>

enum vga_colour_t {
  VGA_COLOR_BLACK = 0,
  VGA_COLOR_BLUE = 1,
  VGA_COLOR_GREEN = 2,
  VGA_COLOR_CYAN = 3,
  VGA_COLOR_RED = 4,
  VGA_COLOR_MAGENTA = 5,
  VGA_COLOR_BROWN = 6,
  VGA_COLOR_LIGHT_GREY = 7,
  VGA_COLOR_DARK_GREY = 8,
  VGA_COLOR_LIGHT_BLUE = 9,
  VGA_COLOR_LIGHT_GREEN = 10,
  VGA_COLOR_LIGHT_CYAN = 11,
  VGA_COLOR_LIGHT_RED = 12,
  VGA_COLOR_LIGHT_MAGENTA = 13,
  VGA_COLOR_YELLOW = 14,
  VGA_COLOR_WHITE = 15,
};

typedef uint8_t vga_char_colour_t;

/*
  Construct a VGA colour code - packed integer with fg in high nibble and bg in
  low nibble
*/
uint8_t vga_char_colour(enum vga_colour_t fg, enum vga_colour_t bg);

/*
  Initializes the VGA buffer to the predefined address, and clears the screen
  Using TTY before calling this function will cause UB
*/
void tty_init();

/*
  Sets the colour for characters to be printed with
*/
void tty_set_colour(vga_char_colour_t colour);

/*
  Writes a character to the VGA buffer at the cursor position
  If `c` is `\n` it will create a newline
*/
void tty_putchar(char c);

/*
  Writes a buffer to the VGA buffer (handles newlines)
*/
void tty_write(const char* data, size_t size);

/*
  Writes a string to the VGA buffer (handles newlines)
*/
void tty_writestring(const char* data);

#endif
