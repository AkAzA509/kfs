#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

typedef struct s_font_info {
	u32_t glyph_count;
	u32_t width;
	u32_t height;
	u32_t headersize;
	u32_t bytesperglyph;
	bool unicode;
} t_font_info;

extern t_font_info font_info;

void flush_rect_fb(u32_t x, u32_t y, u32_t w, u32_t h);
void flush_screen_fb(void);
void scroll_physical_fb(void);
void render_glyph_fb(char c, u8_t color, size_t col, size_t row);
void clear_physical_fb(void);
void update_cursor_fb(void);

#endif // FRAMEBUFFER_H