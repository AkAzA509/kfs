#ifndef KERNEL_INTERNAL_H
#define KERNEL_INTERNAL_H

#include "../includes/stdint.h"
#include "../include/stddef.h"

// terminal size
#define VGA_WIDTH	80
#define VGA_HEIGHT	25
#define VGA_MEMORY	0xB8000

extern size_t	terminal_row;
extern size_t	terminal_column;
extern u8_t		terminal_color;
extern u16_t*	terminal_buffer;

/* Hardware text mode color constants. */
enum vga_color {
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
	VGA_COLOR_LIGHT_BROWN = 14,
	VGA_COLOR_WHITE = 15,
	VGA_COLOR_END
};

u8_t	vga_entry_color(enum vga_color fg, enum vga_color bg);
u16_t	vga_entry(unsigned char uc, u8_t color);
void	set_term_color(u8_t color);

#endif // KERNEL_INTERNAL_H