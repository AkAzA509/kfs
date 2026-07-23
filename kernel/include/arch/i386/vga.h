#ifndef VGA_H
#define VGA_H

#include <stdint.h>
#include <stddef.h>

void	set_cursor(int x, int y);

void	physical_scroll_vga(void);
u16_t	vga_entry(unsigned char uc, u8_t color);
void	putpixel_vga(char c, u8_t color, size_t x, size_t y);
// void	putchar_vga(char c);
// void	scroll_vga(void);
// void	clear_vga(void);
void	clear_physical_vga(void);

#endif // VGA_H