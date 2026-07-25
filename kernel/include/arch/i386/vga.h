#ifndef VGA_H
#define VGA_H

#include <stdint.h>
#include <stddef.h>

u16_t	vga_entry(unsigned char uc, u8_t color);
void	putpixel_vga(char c, u8_t color, size_t x, size_t y);
void	update_cursor_vga(void);
void	scroll_physical_vga(void);
void	clear_physical_vga(void);

#endif // VGA_H