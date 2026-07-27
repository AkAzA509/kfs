#ifndef VGA_H
#define VGA_H

#include <stdint.h>
#include <stddef.h>

u16_t	vga_entry(unsigned char uc, u8_t color);
void	putpixel_vga(char c, u8_t color, size_t x, size_t y);
void	update_cursor_vga(void);
void	scroll_physical_vga(void);
void	clear_physical_vga(void);

// flush_rect_vga() does nothing, it's just to content the display vtable
// and avoid crash when we are in vga mode (assign to NULL a function ptr broke)
void	flush_rect_vga(u32_t x, u32_t y, u32_t w, u32_t h);

// flush_screen_vga() does nothing, it's just to content the display vtable
// and avoid crash when we are in vga mode (assign to NULL a function ptr broke)
void	flush_screen_vga();

#endif // VGA_H