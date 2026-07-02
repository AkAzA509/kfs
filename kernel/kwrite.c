#include "../includes/fonts.h"
#include "../includes/config.h"
#include "terminal.h"
#include "kernel.h"
#include "display.h"

#if VIDEO_MODE == MODE_VGA
void set_cursor(int x, int y)
{
	u16_t pos = y * g_display.width + x;

	outb(0x3D4, 0x0F);
	outb(0x3D5, (u8_t)(pos & 0xFF));
	outb(0x3D4, 0x0E);
	outb(0x3D5, (u8_t)((pos >> 8) & 0xFF));
}

static void	send_char_to_vga(char c, u8_t color, size_t x, size_t y)
{
	const size_t index = y * g_display.width + x;
	g_display.vga_buf[index] = vga_entry(c, color);
}

void putchar_vga(char c)
{
	if (c == '\n') {
		g_display.row++;
		g_display.col = 0;
	}
	else if (c == '\t')
		g_display.col = (g_display.col + 8) & ~7;
	else {
		send_char_to_vga(c, g_display.color, g_display.col, g_display.row);
		if (++g_display.col == g_display.width) {
			g_display.col = 0;
			if (++g_display.row == g_display.height)
				g_display.row = 0;
		}
	}
	set_cursor(g_display.col, g_display.row);
}
#endif

#if VIDEO_MODE == MODE_FRAMEBUFFER

static u32_t	vga_color_to_rgb(enum vga_color color)
{
	static const u32_t palette[16] = {
		0x000000, 0x0000AA, 0x00AA00, 0x00AAAA,
		0xAA0000, 0xAA00AA, 0xAA5500, 0xAAAAAA,
		0x555555, 0x5555FF, 0x55FF55, 0x55FFFF,
		0xFF5555, 0xFF55FF, 0xFFFF55, 0xFFFFFF
	};

	return palette[(u8_t)color & 0x0F];
}

#define pixel u32_t

static void draw_glyph(const u32_t font_width, const u32_t font_height, const u32_t cols, const u32_t rows, u32_t fg, u32_t bg, char c)
{
	const u32_t	bytes_per_glyph = font_height;
	u32_t	glyph_index;
	u8_t	*glyph;
	u8_t	*fb;
	if (fg == 0 || bg == 0) {
		fg = vga_color_to_rgb((enum vga_color)(g_display.color & 0x0F));
		bg = vga_color_to_rgb((enum vga_color)((g_display.color >> 4) & 0x0F));
	}
	
	glyph_index = (u8_t)c;
	if (!(font_header.mode & 1) && glyph_index > 255)
		glyph_index = 0;

	glyph = font_data + 4 + glyph_index * bytes_per_glyph; // rse positione au 1er bit du char a dessiner start + 4 (header) index * (16 * 8)(le nombre de bit du char)
	fb = (u8_t *)g_display.fb_buf;
	for (u32_t y = 0; y < font_height; ++y) {
		u8_t bits = glyph[y];
		u32_t *dst = (u32_t *)(fb + ((g_display.row * font_height + y) * g_display.pitch)
				+ (g_display.col * font_width * sizeof(u32_t)));
		for (u32_t x = 0; x < font_width; ++x)
			dst[x] = (bits & (0x80 >> x)) ? fg : bg;
	}
	if (++g_display.col >= cols) {
		g_display.col = 0;
		if (++g_display.row >= rows)
			g_display.row = 0;
	}
}

void	putchar_framebuffer(char c, u32_t fg, u32_t bg)
{
	const u32_t	font_width = 8;
	const u32_t	font_height = font_header.charsize;
	const u32_t	cols = g_display.width / font_width;
	const u32_t	rows = g_display.height / font_height;

	if (font_height == 0 || cols == 0 || rows == 0 || g_display.bpp != 32)
		return ;
	if (c == '\n') {
		g_display.col = 0;
		if (++g_display.row >= rows)
			g_display.row = 0;
		return ;
	}
	if (c == '\t') {
		g_display.col = (g_display.col + 8) & ~7U;
		if (g_display.col >= cols) {
			g_display.col = 0;
			if (++g_display.row >= rows)
				g_display.row = 0;
		}
		return ;
	}
	draw_glyph(font_width, font_height, cols, rows, fg, bg, c);
}
#endif

void	kputchar(char c)
{
#if VIDEO_MODE == MODE_FRAMEBUFFER
	putchar_framebuffer(c, 0, 0);
#else
	putchar_vga(c);
#endif
}

void	kwrite(const void* data, size_t size)
{
	int tmp = 0;
	const char *str = data;

	for (size_t i = 0; i < size; i++) {
		if (tmp >= 15)
			tmp = 0;
		// set_term_color(vga_entry_color( VGA_COLOR_BLACK, VGA_COLOR_WHITE));
		// set_term_color(vga_entry_color(i % VGA_COLOR_END, VGA_COLOR_BLACK));
		kputchar(str[i]);
		tmp++;
	}
}