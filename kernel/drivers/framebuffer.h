#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

#include "../../includes/stdint.h"
#include "../../includes/stddef.h"

#define PSF1_FONT_MAGIC 0x0436
#define PSF_FONT_MAGIC  0x864ab572

// Pour PSF1 : la largeur du glyphe est toujours 8 bits,
// et la hauteur = charsize
typedef struct {
	u16_t	magic;
	u8_t	mode;
	u8_t	charsize;
}			PSF1_Header;

extern PSF1_Header font_header;

void	putpixel_fb(char c, u8_t color, size_t col, size_t row);
void	putchar_fb(char c);
void	scroll_fb(void);
void	clear_fb(void);
void	clear_physical_fb(void);
void	draw_cursor(int cx, int cy, u32_t color);
void	update_screens(char c);

#endif // FRAMEBUFFER_H