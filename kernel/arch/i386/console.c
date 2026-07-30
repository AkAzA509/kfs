#include <arch/i386/framebuffer.h>
#include <arch/i386/console.h>
#include <arch/i386/vga.h>
#include <kernel/init.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

// #include "kernel/log.h"

// ===== public API , dispatch via vtable ===== //

void set_term_color(u8_t color)
{
	g_screens[current_screen].color = color;
}

u8_t make_color(t_color fg, t_color bg)
{
	return fg | (bg << 4);
}

u32_t color_to_rgb(t_color color)
{
	static const u32_t palette[16] = {
		0x000000, 0x0000AA, 0x00AA00, 0x00AAAA, 0xAA0000, 0xAA00AA,
		0xAA5500, 0xAAAAAA, 0x555555, 0x5555FF, 0x55FF55, 0x55FFFF,
		0xFF5555, 0xFF55FF, 0xFFFF55, 0xFFFFFF
	};
	return palette[(u8_t)color & 0x0F];
}

// check if the current line is in the current screen frame
inline bool line_visible(t_screen_data *s, u32_t line)
{
	return line >= s->view_offset &&
	       line < s->view_offset + g_screen.total_rows;
}

// check if the current line is on the last current screen frame's col
inline bool pinned_to_bottom(t_screen_data *s)
{
	return s->view_offset + g_screen.total_rows - 1 == s->head;
}

inline u8_t get_current_col(void)
{
	return g_screens[current_screen].col;
}

// clear and reinitialize the line when a '\n'
static void clear_line(t_screen_data *s, u32_t line)
{
	u32_t idx = (line % SCROLLBACK_LINES) * SCREEN_COLS;
	memset(&s->text_buf[idx], '\0', g_screen.total_cols);
	memset(&s->color_buf[idx], s->color, g_screen.total_cols);
}

void screen_redraw(void)
{
	t_screen_data *d = &g_screens[current_screen];

	display_d->clear();

	for (u16_t r = 0; r < g_screen.total_rows; r++) {
		u32_t line = d->view_offset + r;
		if (line > d->head)
			break;

		u32_t idx = (line % SCROLLBACK_LINES) * SCREEN_COLS;
		for (u16_t c = 0; c < g_screen.total_cols; c++) {
			if (d->text_buf[idx + c] == '\0')
				continue;
			display_d->putchar_at(d->text_buf[idx + c],
					      d->color_buf[idx + c], c, r);
		}
	}

	display_d->flush_screen();
	display_d->cursor_update();
}

void screen_snap(void)
{
	t_screen_data *s = &g_screens[current_screen];
	u32_t new_offset = (s->head + 1 >= g_screen.total_rows) ?
				   s->head - g_screen.total_rows + 1 :
				   0;

	if (new_offset == s->view_offset)
		return;
	s->view_offset = new_offset;
	screen_redraw();
	// t_screen_data *s = &g_screens[current_screen];

	// 	if (line_visible(s, s->head))
	// 		return ; // déjà sur le direct, rien à faire
	// 	s->view_offset = s->head - g_screen.total_rows + 1;
	// 	screen_redraw();
}

// Detect the line position and from that handle de differents case:
// - scroll and update if the line was the last in the frame
// - if the line overflow the history len, return to the start
// -
static void screen_newline(t_screen_data *s)
{
	bool was_pinned = pinned_to_bottom(s);
	u32_t old_offset = s->view_offset;

	s->head++;
	// check the history bound
	if (s->head - s->view_offset >= SCROLLBACK_LINES)
		s->view_offset = s->head - SCROLLBACK_LINES + 1;
	clear_line(s, s->head);
	s->col = 0;

	if (was_pinned) {
		display_d->scroll();
		s->view_offset++;
		g_screen.cursor_col = g_screen.cursor_row = -1;
	} else if (s->view_offset != old_offset)
		screen_redraw();
}

static void partial_shift(t_screen_data *s, int delta)
{
	u16_t rows = g_screen.total_rows;
	u16_t moved = rows - (u16_t)labs(delta);

	if (g_screen.mode == 1) {
		u16_t *vga = (u16_t *)g_screen.buf;
		if (delta > 0)
			memmove(vga, vga + delta * g_screen.width,
				moved * g_screen.width * sizeof(u16_t));
		else
			memmove(vga - delta * g_screen.width, vga,
				moved * g_screen.width * sizeof(u16_t));
	} else {
		u32_t *back = g_screen.back_buf;
		u32_t row_px = g_screen.width * font_info.height;
		if (delta > 0)
			memmove(back, back + (long)delta * row_px,
				(size_t)moved * row_px * sizeof(u32_t));
		else
			memmove(back - (long)delta * row_px, back,
				(size_t)moved * row_px * sizeof(u32_t));
	}

	u16_t start = (delta > 0) ? rows - delta : 0;
	u16_t end = (delta > 0) ? rows : -delta;
	for (u16_t r = start; r < end; r++) {
		u32_t line = s->view_offset + r;
		u32_t idx = (line % SCROLLBACK_LINES) * SCREEN_COLS;
		for (u16_t c = 0; c < g_screen.total_cols; c++) {
			char ch = s->text_buf[idx + c];
			display_d->putchar_at(ch ? ch : ' ',
					      s->color_buf[idx + c], c, r);
		}
	}
	display_d->flush_screen();
	g_screen.cursor_col = g_screen.cursor_row = -1;
	if (line_visible(s, s->head))
		display_d->cursor_update();
}

void screen_scroll(int step)
{
	t_screen_data *s = &g_screens[current_screen];
	u32_t old_offset = s->view_offset;
	long new_off = (long)old_offset + step;

	long min_off = (long)s->head - SCROLLBACK_LINES + 1;
	if (min_off < 0)
		min_off = 0;

	long max_off = (long)s->head - g_screen.total_rows + 1;
	if (max_off < 0)
		max_off = 0;
	if (new_off > max_off)
		new_off = max_off;
	if (new_off < min_off)
		new_off = min_off;

	int delta = (int)(new_off - (long)old_offset);
	if (delta == 0)
		return;

	s->view_offset = (u32_t)new_off;
	if (labs(delta) >= g_screen.total_rows)
		screen_redraw();
	else
		partial_shift(s, delta);
}

void screen_clear()
{
	t_screen_data *s = &g_screens[current_screen];

	memset(s->text_buf, ' ', sizeof(s->text_buf));
	memset(s->color_buf, s->color, sizeof(s->color_buf));
	s->head = s->view_offset = s->col = 0;
	display_d->clear();
	display_d->cursor_update();
}

int screen_putchar(char c)
{
	t_screen_data *s = &g_screens[current_screen];

	if (c == '\n') {
		screen_newline(s);
		display_d->cursor_update();
		return 1;
	}
	if (c == '\t') {
		u16_t next = (s->col + 8) & ~7U;
		while (s->col < next && s->col < g_screen.total_cols)
			screen_putchar(' ');
		return 1;
	}

	// current pos in the linear buf
	u32_t idx = (s->head % SCROLLBACK_LINES) * SCREEN_COLS + s->col;
	s->text_buf[idx] = c;
	s->color_buf[idx] = s->color;

	if (line_visible(s, s->head)) {
		display_d->putchar_at(c, s->color, s->col,
				      s->head - s->view_offset);

		u16_t row = s->head - s->view_offset;
		display_d->flush_partial(s->col * font_info.width,
					 row * font_info.height,
					 font_info.width, font_info.height);
	}

	s->col++;
	if (s->col >= g_screen.total_cols)
		screen_newline(s);

	display_d->cursor_update();
	return 1;
}

void screen_switch(int new_id)
{
	if (new_id < 0 || new_id >= MAX_SCREENS || new_id == current_screen)
		return;
	current_screen = new_id;
	screen_redraw();
}

// This function set a cursor to the requested position
// Prefer move_cursor() for the arrow deplacement
void move_cursor_to(size_t col)
{
	t_screen_data *s = &g_screens[current_screen];

	s->col = col;
	display_d->cursor_update();
}

void overwrite_at(size_t col, char c)
{
	t_screen_data *s = &g_screens[current_screen];
	u32_t idx = (s->head % SCROLLBACK_LINES) * SCREEN_COLS + col;

	s->text_buf[idx] = c;
	s->color_buf[idx] = s->color;
	if (line_visible(s, s->head)) {
		display_d->putchar_at(c, s->color, col,
				      s->head - s->view_offset);

		u16_t row = s->head - s->view_offset;
		display_d->flush_partial(col * font_info.width,
					 row * font_info.height,
					 font_info.width, font_info.height);
	}
}