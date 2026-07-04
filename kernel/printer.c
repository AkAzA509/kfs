#include "../includes/config.h"
#include "../helpers/helpers.h"
#include "../helpers/kprint/kprint.h"
#include "terminal.h"
#include "kernel.h"
#include "display.h"
#include <stddef.h>

// Put the background color into the 4 upper bits and the foreground color into the 4 lower bits
inline u8_t vga_entry_color(enum vga_color fg, enum vga_color bg)
{
	return fg | bg << 4;
}

// add the char and the color bytes prepared by vga_entry_color()
inline u16_t vga_entry(unsigned char uc, u8_t color)
{
	return (u16_t) uc | (u16_t) color << 8;
}

// ===== VGA mode ===== //
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
	g_display.buf.vga_buf[index] = vga_entry(c, color);
}

static void	scroll_vga()
{
	for (size_t y = 1; y < g_display.height; ++y) {
		for (size_t x = 0; x < g_display.width; ++x)
			send_char_to_vga(g_display.buf.vga_buf[y * g_display.width + x], g_display.color, x, y - 1);
	}

	for (size_t x = 0; x < g_display.width; ++x) {
		send_char_to_vga(' ', g_display.color, x, g_display.height - 1);
	}

	g_display.row = g_display.height - 1;
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
			// if (++g_display.row == g_display.height)
				g_display.row++;
		}
	}
	// set_cursor(g_display.col, g_display.row);
}
#endif

// ===== Framebuffer mode, glyph drawing ===== //
#if VIDEO_MODE == MODE_FRAMEBUFFER

static void swap_rect(u32_t x, u32_t y, u32_t width, u32_t height)
{
	u32_t *front = (u32_t *)(u32_t)g_display.buf.fb_buf;
	u32_t *back = g_display.back_buf;
	const size_t front_stride = g_display.pitch / sizeof(u32_t);

	for (u32_t row = 0; row < height; ++row) {
		u32_t *dst = front + (y + row) * front_stride + x;
		u32_t *src = back + (y + row) * g_display.width + x;
		memcpy(dst, src, width * sizeof(u32_t));
	}
}

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

static void draw_cursor(int cx, int cy, u32_t color)
{
	for (int x = cx * 8; x < cx * 8 + 8; x++)
		for (int y = cy * 16 + 14; y < cy * 16 + 16; y++)
			((u32_t*)(u32_t)g_display.buf.fb_buf)[y * g_display.pitch / 4 + x] = color;
}

#define pixel u32_t

static void draw_glyph(const u32_t font_width, const u32_t font_height, const u32_t cols, const u32_t rows, u32_t fg, u32_t bg, char c)
{
	const u32_t	bytes_per_glyph = font_height;
	u32_t	glyph_index;
	u8_t	*glyph;
	u32_t	*fb;
	const u32_t origin_x = g_display.col * font_width;
	const u32_t origin_y = g_display.row * font_height;
	if (fg == 0 || bg == 0) {
		fg = vga_color_to_rgb((enum vga_color)(g_display.color & 0x0F));
		bg = vga_color_to_rgb((enum vga_color)((g_display.color >> 4) & 0x0F));
	}
	
	glyph_index = (u8_t)c;
	if (!(font_header.mode & 1) && glyph_index > 255)
		glyph_index = 0;

	glyph = font_data + 4 + glyph_index * bytes_per_glyph; // se positione au 1er bit du char a dessiner start + 4 (header) index * (16 * 8)(le nombre de bit du char)
	fb = g_display.back_buf;
	for (u32_t y = 0; y < font_height; ++y) {
		u8_t bits = glyph[y];
		u32_t *dst = fb + ((origin_y + y) * g_display.width) + origin_x;
		for (u32_t x = 0; x < font_width; ++x)
			dst[x] = (bits & (0x80 >> x)) ? fg : bg;
	}
	if (++g_display.col >= cols) {
		g_display.col = 0;
		g_display.row++;
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
		g_display.row++;
	}
	else if (c == '\t') {
		g_display.col = (g_display.col + 8) & ~7U;
		if (g_display.col >= cols) {
			g_display.col = 0;
			g_display.row++;
		}
	}
	else
		draw_glyph(font_width, font_height, cols, rows, fg, bg, c);
}

static void	scroll_fb()
{
	const u32_t font_height	= font_header.charsize;
	const u32_t line_size	= g_display.width * font_height; // nb of pixel in a line (offset)

	// move the buffer to 1 line up
	memmove(g_display.back_buf,
		g_display.back_buf + line_size,
		g_display.width * (g_display.height - font_height) * sizeof(u32_t));

	// empty the last line
	memset(g_display.back_buf + g_display.width * (g_display.height - font_height),
		0,
		line_size * sizeof(u32_t));

	const u32_t rows = g_display.height / font_header.charsize;
	g_display.row = rows - 1;
}
#endif

// ===== Public API ===== //
void	update_cursor()
{
	#if VIDEO_MODE == MODE_FRAMEBUFFER
		draw_cursor(g_display.col, g_display.row, 0xFFFFFF);
	#else
		set_cursor(g_display.col, g_display.row);
	#endif
}

void flush(void)
{
	#if VIDEO_MODE == MODE_FRAMEBUFFER
		swap_rect(0, 0, g_display.width, g_display.height);
	#endif
	update_cursor();
}

void	scroll()
{
	#if VIDEO_MODE == MODE_FRAMEBUFFER
		scroll_fb();
	#else
		scroll_vga();
	#endif
	flush();
}