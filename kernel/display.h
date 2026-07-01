#ifndef DISPLAY_H
#define DISPLAY_H

#include "../includes/stdint.h"
#include "../includes/stddef.h"
#include "multiboot.h"

// Hardware text mode color constants
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

#define PSF1_FONT_MAGIC 0x0436

/*
	For PSF1 glyph width is always = 8 bits
	and glyph height = characterSize
*/
typedef struct {
	u16_t	magic;
	u8_t	mode;
	u8_t	charsize;
}			PSF1_Header;

extern PSF1_Header font_header;

#define PSF_FONT_MAGIC 0x864ab572

typedef struct s_display {
	volatile u16_t	*vga_buf;
	volatile u32_t	*fb_buf;
	size_t			col;
	size_t			row;
	int				mode;
	u32_t			width;
	u32_t			height;
	u32_t			pitch;
	u8_t			bpp;
	u8_t			color;
}					t_display;

extern t_display	g_display;

#define DISPLAY_VGA	0
#define DISPLAY_FB	1

void	debug_diplay();

void	init_term(void);
void	init_display(multiboot_info *mbi);

// Buffer and color

u8_t	vga_entry_color(enum vga_color fg, enum vga_color bg);
// void	send_char_to_vga(char c, u8_t color, size_t x, size_t y);
u16_t	vga_entry(unsigned char uc, u8_t color);
void	set_term_color(u8_t color);
void	set_cursor(int x, int y);
void	putchar_vga(char c);
void	putchar_framebuffer(char c, u32_t fg, u32_t bg);


#endif // DISPLAY_H