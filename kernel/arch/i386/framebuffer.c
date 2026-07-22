#include <arch/i386/framebuffer.h>
#include <arch/i386/tty.h>
#include <kernel/init.h>
#include <string.h>

// Copy the back buffer into the front buffer, to avoid flickering
// front_stride = number of real pixels between 2 lines in the VRAM (maybe > width because of hardware alignment => pitch)
// back_buf is a compact buffer without padding, so width is use directly
static void swap_rect(u32_t x, u32_t y, u32_t width, u32_t height)
{
	const	size_t front_stride = g_screen.pitch / sizeof(u32_t);
	u32_t	*front = (u32_t *)g_screen.buf;
	u32_t	*back = g_screen.back_buf;

	for (u32_t row = 0; row < height; ++row) {
		u32_t	*dst = front + (y + row) * front_stride + x;
		u32_t	*src = back + (y + row) * g_screen.width + x;
		memcpy(dst, src, width * sizeof(u32_t));
	}
}

void draw_cursor(int cx, int cy, u32_t color)
{
	const	int start_x = cx * 8;
	const	int start_y = cy * 16 + 14;

	if (cx == g_screen.cursor_col && cy == g_screen.cursor_row)
		return ;

	if (g_screen.cursor_col >= 0)
		swap_rect(g_screen.cursor_col * 8, g_screen.cursor_row * 16 + 14, 8, 16);

	if (start_x < 0 || start_y < 0 || (u32_t)(start_x + 8) > g_screen.width ||
		(u32_t)(start_y + 2) > g_screen.height) {
		g_screen.cursor_col = g_screen.cursor_row = -1;
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
	const u32_t	font_width = font_info.width;
	const u32_t	font_height = font_info.height;
	const u32_t	origin_x = col * font_width;	// col offset
	const u32_t	origin_y = row * font_height;	// raw offset
	u32_t	fg = color_to_rgb((t_color)(color & 0x0F));
	u32_t	bg = color_to_rgb((t_color)((color >> 4) & 0x0F));
	u32_t	glyph_index;
	u32_t	*back;
	u8_t	*glyph;

	glyph_index = (u8_t)c;
	if (glyph_index > font_info.glyph_count)
		glyph_index = 0;

	// font buffer + 4 (font header) + glyph_index * font_height (16)
	// = index of the char's first bit into font_buffer
	glyph = font_data + font_info.headersize + glyph_index * font_height;
	back = g_screen.back_buf;


	for (u32_t y = 0; y < font_height; ++y) {
		// take the whole row of the font char (8bits packed in one byte)
		u8_t byte = glyph[y];

		// offset into back memory
		u32_t *dst = back + ((origin_y + y) * g_screen.width) + origin_x;
		for (u32_t x = 0; x < font_width; ++x)
			// test each bit of the byte row
			// the mask test the leftover bit (1 fg, 0 bg)
			dst[x] = (byte & (0x80 >> x)) ? fg : bg;
	}

	swap_rect(origin_x, origin_y, font_width, font_height);
}

void	update_screens(char c)
{
	size_t idx = g_screen.row * SCREEN_COLS + g_screen.col;

	g_screens[current_screen].text_buf[idx] = c;
	g_screens[current_screen].color_buf[idx] = g_screen.color;
}

static void draw_glyph(const u32_t cols, char c)
{
	putpixel_fb(c, g_screen.color, g_screen.col, g_screen.row);

	update_screens(c);

	if (++g_screen.col >= cols) {
		g_screen.col = 0;
		g_screen.row++;
	}
}

void	putchar_fb(char c)
{
	if (c == '\n') {
		g_screen.col = 0;
		g_screen.row++;
	}
	else if (c == '\t') {
		g_screen.col = (g_screen.col + 8) & ~7U;
		if (g_screen.col >= g_screen.total_cols) {
			g_screen.col = 0;
			g_screen.row++;
		}
	}
	else
		draw_glyph(g_screen.total_cols, c);

	if (g_screen.row >= g_screen.total_rows)
		scroll_fb();

	g_screens[current_screen].col = g_screen.col;
	g_screens[current_screen].row = g_screen.row;
}

void	scroll_fb(void)
{
	const u32_t	font_height = font_info.height;
	const u32_t	line_size = g_screen.width * font_height;
	u32_t	*back = g_screen.back_buf;

	if (font_height == 0)
		return ;

	// move the memory up by 1 row
	memmove(back, back + line_size,
		g_screen.width * (g_screen.height - font_height) * sizeof(u32_t));
	// clear the "new" line down
	memset(back + g_screen.width * (g_screen.height - font_height), 0,
		line_size * sizeof(u32_t));

	// update the back screen
	t_screen_data *d = &g_screens[current_screen];
	for (u16_t r = 0; r < g_screen.total_rows - 1; r++) {
		for (u16_t c = 0; c < g_screen.total_cols; c++) {
			size_t	dst = r * SCREEN_COLS + c;
			size_t	src = (r + 1) * SCREEN_COLS + c;
			d->text_buf[dst] = d->text_buf[src];
			d->color_buf[dst] = d->color_buf[src];
		}
	}

	// clear the "new" line down
	for (u16_t c = 0; c < g_screen.total_cols; ++c) {
		size_t	idx = (g_screen.total_rows - 1) * SCREEN_COLS + c;
		d->text_buf[idx] = ' ';
		d->color_buf[idx] = g_screen.color;
	}

	g_screen.row = g_screen.total_rows - 1;
	swap_rect(0, 0, g_screen.width, g_screen.height);
	g_screen.cursor_col = g_screen.cursor_row = -1;
}

// clear the back buf who been swap with the front
void	clear_physical_fb(void)
{
	const u32_t	total_pixels = g_screen.width * g_screen.height;
	u32_t	*back = g_screen.back_buf;

	for (u32_t i = 0; i < total_pixels; ++i)
		back[i] = 0x000000;

	swap_rect(0, 0, g_screen.width, g_screen.height);
	g_screen.cursor_col = g_screen.cursor_row = -1;
	g_screen.col = g_screen.row = 0;
}

// call the back buf cleen and clean the back screen
void	clear_fb(void)
{
	clear_physical_fb();

	t_screen_data	*d = &g_screens[current_screen];

	for (u16_t r = 0; r < g_screen.total_rows; r++) {
		for (u16_t c = 0; c <g_screen.total_cols; c++) {
			size_t	idx = r * SCREEN_COLS + c;
			d->text_buf[idx] = ' ';
			d->color_buf[idx] = g_screen.color;
		}
	}
}