#include "../includes/stdint.h"
#include "kernel_internal.h"

void set_term_color(u8_t color)
{
	t_color = color;
}

// Put the background color into the 4 upper bits and the font color into the 4 lower bits
inline u8_t vga_entry_color(enum vga_color fg, enum vga_color bg)
{
	return fg | bg << 4;
}

// add the char and the color bytes prepared by vga_entry_color()
inline u16_t vga_entry(unsigned char uc, u8_t color)
{
	return (u16_t) uc | (u16_t) color << 8;
}