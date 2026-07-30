#include <arch/i386/framebuffer.h>
#include <arch/i386/console.h>
#include <kernel/log.h>
#include <string.h>

// Copy the back buffer into the front buffer, to avoid flickering
// front_stride = number of real pixels between 2 lines in the VRAM (maybe > width because of hardware alignment => pitch)
// back_buf is a compact buffer without padding, so width is use directly
static void swap_rect(u32_t x, u32_t y, u32_t width, u32_t height)
{
	const size_t front_stride = g_screen.pitch / sizeof(u32_t);
	u32_t *front = (u32_t *)g_screen.buf;
	u32_t *back = g_screen.back_buf;

	for (u32_t row = 0; row < height; ++row) {
		u32_t *dst = front + (y + row) * front_stride + x;
		u32_t *src = back + (y + row) * g_screen.width + x;
		memcpy(dst, src, width * sizeof(u32_t));
	}
}

static void draw_cursor(int cx, int cy, u32_t color)
{
	const int start_x = cx * font_info.width;
	const int start_y = cy * font_info.height + 14;

	// klog("draw_cursor READ: &cursor_col=%p\n", (void *)&g_screen.cursor_col);
	// klog("draw_cursor: cx=%d cy=%d | old_col=%d old_row=%d\n", cx, cy, g_screen.cursor_col, g_screen.cursor_row);

	if (cx == g_screen.cursor_col && cy == g_screen.cursor_row)
		return;

	if (g_screen.cursor_col >= 0)
		swap_rect(g_screen.cursor_col * font_info.width,
			  g_screen.cursor_row * font_info.height + 14,
			  font_info.width, 2);

	if (start_x < 0 || start_y < 0 ||
	    (u32_t)(start_x + 8) > g_screen.width ||
	    (u32_t)(start_y + 2) > g_screen.height) {
		g_screen.cursor_col = g_screen.cursor_row = -1;
		// klog("draw_cursor BOUND FAIL: start_x=%d start_y=%d width=%u height=%u\n",
		// 				start_x, start_y, g_screen.width, g_screen.height);
		return;
	}

	u32_t *front = (u32_t *)g_screen.buf;

	for (int x = start_x; x < start_x + 8; x++)
		for (int y = start_y; y < start_y + 2; y++)
			front[y * (g_screen.pitch / 4) + x] = color;

	g_screen.cursor_col = cx;
	g_screen.cursor_row = cy;
	// klog("draw_cursor SET: &cursor_col=%p wrote col=%d row=%d\n",
	// (void *)&g_screen.cursor_col, g_screen.cursor_col, g_screen.cursor_row);
}

void update_cursor_fb(void)
{
	t_screen_data *s = &g_screens[current_screen];
	int col = s->col;
	int row = s->head - s->view_offset;

	u8_t bg_index = (s->color >> 4) & 0x0F;
	u32_t bg_rgb = color_to_rgb((t_color)bg_index);
	u32_t cursor_color = ~bg_rgb & 0x00FFFFFF;
	draw_cursor(col, row, cursor_color);
}

void render_glyph_fb(char c, u8_t color, size_t col, size_t row)
{
	const u32_t font_width = font_info.width;
	const u32_t font_height = font_info.height;
	const u32_t origin_x = col * font_width;
	const u32_t origin_y = row * font_height;
	u32_t fg = color_to_rgb((t_color)(color & 0x0F));
	u32_t bg = color_to_rgb((t_color)((color >> 4) & 0x0F));
	u32_t glyph_index = (u8_t)c;

	if (glyph_index >= font_info.glyph_count)
		glyph_index = 0;

	u8_t *glyph =
		font_data + font_info.headersize + glyph_index * font_height;
	u32_t *back = g_screen.back_buf;

	for (u32_t y = 0; y < font_height; ++y) {
		u8_t byte = glyph[y];
		u32_t *dst = back + (origin_y + y) * g_screen.width + origin_x;
		for (u32_t x = 0; x < font_width; ++x)
			dst[x] = (byte & (0x80 >> x)) ? fg : bg;
	}
}

// sync explicite, appelée UNE fois par opération logique, jamais par glyphe
void flush_rect_fb(u32_t x, u32_t y, u32_t w, u32_t h)
{
	swap_rect(x, y, w, h);
}

void flush_screen_fb(void)
{
	swap_rect(0, 0, g_screen.width, g_screen.height);
}

void scroll_physical_fb(void)
{
	const u32_t font_height = font_info.height;
	const u32_t row_pixels = g_screen.width * font_height;
	u32_t *back = g_screen.back_buf;

	memmove(back, back + row_pixels,
		g_screen.width * (g_screen.height - font_height) *
			sizeof(u32_t));
	memset(back + g_screen.width * (g_screen.height - font_height), 0,
	       row_pixels * sizeof(u32_t));

	flush_screen_fb(); // un seul sync, pas un par ligne
}

// clear the back buf who been swap with the front
void clear_physical_fb(void)
{
	const u32_t total_pixels = g_screen.width * g_screen.height;
	u32_t *back = g_screen.back_buf;
	// u32_t	fg = color_to_rgb((t_color)(color & 0x0F));
	u32_t bg = color_to_rgb(
		(t_color)((g_screens[current_screen].color >> 4) & 0x0F));

	for (u32_t i = 0; i < total_pixels; ++i)
		back[i] = bg;

	swap_rect(0, 0, g_screen.width, g_screen.height);
	g_screen.cursor_col = g_screen.cursor_row = -1;
}