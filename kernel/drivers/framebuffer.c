#include "../../helpers/helpers.h"
#include "framebuffer.h"
#include "../terminal.h"
#include "../init.h"

static void swap_rect(u32_t x, u32_t y, u32_t width, u32_t height)
{
	u32_t *front = (u32_t *)g_screen.buf;
	u32_t *back = g_screen.back_buf;
	const size_t front_stride = g_screen.pitch / sizeof(u32_t);

	for (u32_t row = 0; row < height; ++row) {
		u32_t *dst = front + (y + row) * front_stride + x;
		u32_t *src = back + (y + row) * g_screen.width + x;
		memcpy(dst, src, width * sizeof(u32_t));
	}
}

void draw_cursor(int cx, int cy, u32_t color)
{
	const int start_x = cx * 8;
	const int start_y = cy * 16 + 14;

	if (cx == g_screen.cursor_col && cy == g_screen.cursor_row)
		return ;

	if (g_screen.cursor_col >= 0)
		swap_rect(g_screen.cursor_col * 8, g_screen.cursor_row * 16 + 14, 8, 16);

	if (start_x < 0 || start_y < 0 ||
		(u32_t)(start_x + 8) > g_screen.width || (u32_t)(start_y + 2) > g_screen.height) {
		g_screen.cursor_col = -1;
		g_screen.cursor_row = -1;
		return ;
	}

	u32_t *front = (u32_t *)g_screen.buf;
	for (int x = start_x; x < start_x + 8; x++)
		for (int y = start_y; y < start_y + 2; y++)
			front[y * (g_screen.pitch / 4) + x] = color;

	g_screen.cursor_col = cx;
	g_screen.cursor_row = cy;
}

void	putpixel_fb(char c, u8_t color, size_t col, size_t row)
{
	const u32_t font_width = 8;
	const u32_t font_height = font_header.charsize;
	const u32_t bytes_per_glyph = font_height;
	const u32_t origin_x = col * font_width;
	const u32_t origin_y = row * font_height;
	u32_t glyph_index;
	u8_t *glyph;
	u32_t *back;

	u32_t fg = color_to_rgb((t_color)(color & 0x0F));
	u32_t bg = color_to_rgb((t_color)((color >> 4) & 0x0F));

	glyph_index = (u8_t)c;
	if (!(font_header.mode & 1) && glyph_index > 255)
		glyph_index = 0;

	glyph = font_data + 4 + glyph_index * bytes_per_glyph;
	back = g_screen.back_buf;

	for (u32_t y = 0; y < font_height; ++y) {
		u8_t bits = glyph[y];
		u32_t *dst = back + ((origin_y + y) * g_screen.width) + origin_x;
		for (u32_t x = 0; x < font_width; ++x)
			dst[x] = (bits & (0x80 >> x)) ? fg : bg;
	}

	swap_rect(origin_x, origin_y, font_width, font_height);
}

static void draw_glyph(const u32_t cols, char c)
{
	putpixel_fb(c, g_screen.color, g_screen.col, g_screen.row);

	size_t idx = g_screen.row * SCREEN_COLS + g_screen.col;
	g_screens[current_screen].text_buf[idx] = c;
	g_screens[current_screen].color_buf[idx] = g_screen.color;

	if (++g_screen.col >= cols) {
		g_screen.col = 0;
		g_screen.row++;
	}
}

void	putchar_fb(char c)
{
	const u32_t	font_width = 8;
	const u32_t	font_height = font_header.charsize;
	const u32_t	cols = g_screen.width / font_width;
	const u32_t	rows = g_screen.height / font_height;

	if (font_height == 0 || cols == 0 || rows == 0 || g_screen.bpp != 32)
		return ;

	if (c == '\n') {
		g_screen.col = 0;
		g_screen.row++;
	}
	else if (c == '\t') {
		g_screen.col = (g_screen.col + 8) & ~7U;
		if (g_screen.col >= cols) {
			g_screen.col = 0;
			g_screen.row++;
		}
	}
	else
		draw_glyph(cols, c);

	if (g_screen.row >= rows)
		scroll_fb();

	g_screens[current_screen].col = g_screen.col;
	g_screens[current_screen].row = g_screen.row;
}

// void	scroll_fb(void)
// {
// 	const u32_t font_height = font_header.charsize;
// 	const u32_t line_size = g_screen.width * font_height;
// 	u32_t *back = g_screen.back_buf;

// 	if (font_height == 0)
// 		return ;

// 	memmove(back,
// 		back + line_size,
// 		g_screen.width * (g_screen.height - font_height) * sizeof(u32_t));

// 	memset(back + g_screen.width * (g_screen.height - font_height),
// 		0,
// 		line_size * sizeof(u32_t));

// 	const u32_t rows = g_screen.height / font_header.charsize;
// 	g_screen.row = rows - 1;
// 	swap_rect(0, 0, g_screen.width, g_screen.height);
// 	g_screen.cursor_col = -1;
// 	g_screen.cursor_row = -1;
// }

void	scroll_fb(void)
{
	const	u32_t font_height = font_header.charsize;
	const	u32_t line_size = g_screen.width * font_height;
	const	u32_t cols = g_screen.width / 8;
	const	u32_t rows = g_screen.height / font_height;
	u32_t	*back = g_screen.back_buf;

	if (font_height == 0)
		return ;

	memmove(back, back + line_size,
		g_screen.width * (g_screen.height - font_height) * sizeof(u32_t));
	memset(back + g_screen.width * (g_screen.height - font_height), 0,
		line_size * sizeof(u32_t));

	t_screen_data *d = &g_screens[current_screen];
	for (u32_t r = 0; r < rows - 1; r++) {
		for (u32_t c = 0; c < cols; c++) {
			size_t dst = r * SCREEN_COLS + c;
			size_t src = (r + 1) * SCREEN_COLS + c;
			d->text_buf[dst] = d->text_buf[src];
			d->color_buf[dst] = d->color_buf[src];
		}
	}
	for (u32_t c = 0; c < cols; ++c) {
		size_t idx = (rows - 1) * SCREEN_COLS + c;
		d->text_buf[idx] = ' ';
		d->color_buf[idx] = g_screen.color;
	}

	g_screen.row = rows - 1;
	swap_rect(0, 0, g_screen.width, g_screen.height);
	g_screen.cursor_col = -1;
	g_screen.cursor_row = -1;
}

// void	clear_fb(void)
// {
// 	u32_t *back = g_screen.back_buf;
// 	const u32_t total_pixels = g_screen.width * g_screen.height;

// 	for (u32_t i = 0; i < total_pixels; ++i)
// 		back[i] = 0x000000;

// 	swap_rect(0, 0, g_screen.width, g_screen.height);
// 	g_screen.col = 0;
// 	g_screen.row = 0;
// 	g_screen.cursor_col = -1;
// 	g_screen.cursor_row = -1;
// }

void	clear_physical_fb(void)
{
	u32_t *back = g_screen.back_buf;
	const u32_t total_pixels = g_screen.width * g_screen.height;

	for (u32_t i = 0; i < total_pixels; ++i)
		back[i] = 0x000000;

	swap_rect(0, 0, g_screen.width, g_screen.height);
	g_screen.col = 0;
	g_screen.row = 0;
	g_screen.cursor_col = -1;
	g_screen.cursor_row = -1;
}

// Clear "complet" (vtable) : physique + données logiques de l'écran actif
void	clear_fb(void)
{
	clear_physical_fb();

	const u32_t cols = g_screen.width / 8;
	const u32_t rows = g_screen.height / font_header.charsize;
	t_screen_data *d = &g_screens[current_screen];

	for (u32_t r = 0; r < rows; r++) {
		for (u32_t c = 0; c < cols; c++) {
			size_t idx = r * SCREEN_COLS + c;
			d->text_buf[idx] = ' ';
			d->color_buf[idx] = g_screen.color;
		}
	}
}
