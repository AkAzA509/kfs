#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

#include "stdbool.h"
#include <stdint.h>
#include <stddef.h>

typedef struct s_font_info {
	u32_t	glyph_count;
	u32_t	width;
	u32_t	height;
	u32_t	headersize;
	u32_t	bytesperglyph;	/* size of each glyph */
	bool	unicode;
}				t_font_info;

extern t_font_info font_info;

void	putpixel_fb(char c, u8_t color, size_t col, size_t row);
void	putchar_fb(char c);
void	scroll_fb(void);
void	clear_fb(void);
void	clear_physical_fb(void);
void	draw_cursor(int cx, int cy, u32_t color);
void	update_screens(char c);

#endif // FRAMEBUFFER_H