#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <arch/i386/framebuffer.h>
#include <arch/i386/vga.h>
#include <arch/i386/tty.h>
#include <kernel/init.h>

// ===== public API , dispatch via vtable ===== //

void set_term_color(u8_t color)
{
	g_screens[current_screen].color = color;
}

u8_t	make_color(t_color fg, t_color bg)
{
	return fg | (bg << 4);
}

u32_t	color_to_rgb(t_color color)
{
	static const u32_t palette[16] = {
		0x000000, 0x0000AA, 0x00AA00, 0x00AAAA,
		0xAA0000, 0xAA00AA, 0xAA5500, 0xAAAAAA,
		0x555555, 0x5555FF, 0x55FF55, 0x55FFFF,
		0xFF5555, 0xFF55FF, 0xFFFF55, 0xFFFFFF
	};
	return palette[(u8_t)color & 0x0F];
}

void	update_cursor(void)
{
	int	col = g_screens[current_screen].col;
	int	row = g_screens[current_screen].head;

	if (g_screen.mode == 0) {
		u8_t	bg_index = (g_screens[current_screen].color >> 4) & 0x0F;
		u32_t	bg_rgb = color_to_rgb((t_color)bg_index);
		u32_t	cursor_color = ~bg_rgb & 0x00FFFFFF;
		draw_cursor(col, row, cursor_color);
	}
	else
		set_cursor(col, row);
}

static inline bool line_visible(t_screen_data *s, u32_t line)
{
	return line >= s->view_offset && line < s->view_offset + g_screen.total_rows;
}

static inline bool pinned_to_bottom(t_screen_data *s)
{
	return s->view_offset + g_screen.total_rows - 1 == s->head;
}

static void clear_line(t_screen_data *s, u32_t line)
{
	u32_t	idx = (line % SCROLLBACK_LINES) * SCREEN_COLS;
	memset(&s->text_buf[idx], ' ', g_screen.total_cols);
	memset(&s->color_buf[idx], g_screens[current_screen].color, g_screen.total_cols);
}

static void screen_redraw(void)
{
	t_screen_data	*d = &g_screens[current_screen];

	current_driver->clear();

	for (u16_t r = 0; r < g_screen.total_rows; r++) {
		u32_t	line = d->view_offset + r;
		if (line > d->head)
			break ;
		u32_t	idx = (line % SCROLLBACK_LINES) * SCREEN_COLS;
		for (u16_t c = 0; c < g_screen.total_cols; c++) {
			if (d->text_buf[idx + c] == '\0')
				continue ;
			current_driver->putchar_at(d->text_buf[idx + c], d->color_buf[idx + c], c, r);
			// if (g_screen.mode == 1)
			// 	putpixel_vga(d->text_buf[idx + c], d->color_buf[idx + c], c, r);
			// else
			// 	render_glyph_fb(d->text_buf[idx + c], d->color_buf[idx + c], c, r); // pas de flush ici
		}
	}
	if (g_screen.mode == 0)
		flush_screen_fb(); // UN seul memcpy global, pas 12000

	g_screens[current_screen].col = d->col;
	g_screens[current_screen].head = (u16_t)(d->head - d->view_offset);
	g_screens[current_screen].color = d->color;
	update_cursor();
}

void screen_snap(void)
{
	t_screen_data	*s = &g_screens[current_screen];
	u32_t	new_offset = (s->head + 1 >= g_screen.total_rows) ?
						s->head - g_screen.total_rows + 1 : 0;

	if (new_offset == s->view_offset)
		return ;
	s->view_offset = new_offset;
	screen_redraw();
}

static void screen_newline(t_screen_data *s)
{
	bool	was_pinned = pinned_to_bottom(s);

	s->head++;
	if (s->head - s->view_offset >= SCROLLBACK_LINES)
		s->view_offset = s->head - SCROLLBACK_LINES + 1;
	clear_line(s, s->head);

	g_screens[current_screen].col = 0;
	s->col = 0;

	if (!was_pinned)
		return ; // pas visible, rien à faire — déjà optimal ici

	current_driver->scroll();
	s->view_offset++;
	g_screen.cursor_col = g_screen.cursor_row = -1; // invalide le cache du curseur fb
}

static void partial_shift(t_screen_data *s, int delta)
{
	u16_t	rows = g_screen.total_rows;
	u16_t	moved = rows - (u16_t)labs(delta);

	if (g_screen.mode == 1) {
		u16_t	*vga = (u16_t *)g_screen.buf;
		if (delta > 0)
			memmove(vga, vga + delta * g_screen.width, moved * g_screen.width * sizeof(u16_t));
		else
			memmove(vga - delta * g_screen.width, vga, moved * g_screen.width * sizeof(u16_t));
	} else {
		u32_t	*back = g_screen.back_buf;
		u32_t	row_px = g_screen.width * font_info.height;
		if (delta > 0)
			memmove(back, back + (long)delta * row_px, (size_t)moved * row_px * sizeof(u32_t));
		else
			memmove(back - (long)delta * row_px, back, (size_t)moved * row_px * sizeof(u32_t));
	}

	// ne rend QUE les lignes nouvellement exposées, pas tout l'écran
	u16_t	start = (delta > 0) ? rows - delta : 0;
	u16_t	end = (delta > 0) ? rows : -delta;
	for (u16_t r = start; r < end; r++) {
		u32_t	line = s->view_offset + r;
		u32_t	idx = (line % SCROLLBACK_LINES) * SCREEN_COLS;
		for (u16_t c = 0; c < g_screen.total_cols; c++) {
			current_driver->putchar_at(s->text_buf[idx + c], s->color_buf[idx + c], c, r);
			// if (g_screen.mode == 1)
			// 	putpixel_vga(s->text_buf[idx + c], s->color_buf[idx + c], c, r);
			// else
			// 	render_glyph_fb(s->text_buf[idx + c], s->color_buf[idx + c], c, r);
		}
	}
	if (g_screen.mode == 0)
		flush_screen_fb(); // un seul sync pour tout le shift + les lignes neuves
}

void screen_scroll(int delta)
{
	t_screen_data	*s = &g_screens[current_screen];
	long	max_off = (long)s->head - g_screen.total_rows + 1;
	long	min_off = (long)s->head - SCROLLBACK_LINES + 1;

	if (max_off < 0)
		max_off = 0;
	if (min_off < 0)
		min_off = 0;

	long	new_off = s->view_offset + delta;
	if (new_off > max_off)
		new_off = max_off;
	if (new_off < min_off)
		new_off = min_off;

	long	actual = new_off - (long)s->view_offset;
	if (actual == 0)
		return ;

	// saut trop grand : le partiel coûterait plus cher qu'un redraw complet
	if (labs(actual) >= g_screen.total_rows) {
		s->view_offset = (u32_t)new_off;
		screen_redraw();
		return ;
	}

	s->view_offset = (u32_t)new_off;
	partial_shift(s, (int)actual);
}

void screen_clear(bool full)
{
	t_screen_data	*s = &g_screens[current_screen];

	if (full) {
		memset(s->text_buf, ' ', sizeof(s->text_buf));
		memset(s->color_buf, s->color, sizeof(s->color_buf));
		s->head = s->view_offset = s->col = 0;
		g_screens[current_screen].col = 0;
	}
	else {
		for (u16_t r = 0; r < g_screen.total_rows; r++)
			clear_line(s, s->view_offset + r);
	}
	current_driver->clear();
	update_cursor();
}

int kputchar(char c)
{
	t_screen_data	*s = &g_screens[current_screen];

	if (c == '\n') {
		screen_newline(s);
		update_cursor();
		return 1;
	}
	if (c == '\t') {
		u16_t next = (g_screens[current_screen].col + 8) & ~7U;
		while (g_screens[current_screen].col < next && g_screens[current_screen].col < g_screen.total_cols)
			kputchar(' ');
		return 1;
	}

	u32_t	idx = (s->head % SCROLLBACK_LINES) * SCREEN_COLS + g_screens[current_screen].col;
	s->text_buf[idx] = c;
	s->color_buf[idx] = g_screens[current_screen].color;

	if (line_visible(s, s->head)) {
		current_driver->putchar_at(c, g_screens[current_screen].color, g_screens[current_screen].col, s->head - s->view_offset);
		if (g_screen.mode == 0) {
			u16_t	row = s->head - s->view_offset;
			flush_rect_fb(g_screens[current_screen].col * font_info.width, row * font_info.height,
						font_info.width, font_info.height);
		}
	}

	g_screens[current_screen].col++;
	s->col = g_screens[current_screen].col;
	if (g_screens[current_screen].col >= g_screen.total_cols)
		screen_newline(s);

	update_cursor();
	return 1;
}

void screen_switch(int new_id)
{
	if (new_id < 0 || new_id >= MAX_SCREENS || new_id == current_screen)
		return ;
	current_screen = new_id;
	screen_redraw();
}

void backspace(void)
{
	t_screen_data	*s = &g_screens[current_screen];

	if (g_screens[current_screen].col == 0)
		return ; // la ligne discipline (tty input) doit empêcher de remonter avant le début de l'input
	g_screens[current_screen].col--;
	s->col = g_screens[current_screen].col;

	u32_t	idx = (s->head % SCROLLBACK_LINES) * SCREEN_COLS + g_screens[current_screen].col;
	s->text_buf[idx] = ' ';
	s->color_buf[idx] = g_screens[current_screen].color;

	if (line_visible(s, s->head)) {
		current_driver->putchar_at(' ', g_screens[current_screen].color, g_screens[current_screen].col, s->head - s->view_offset);
		if (g_screen.mode == 0) {
			u16_t	row = s->head - s->view_offset;
			flush_rect_fb(g_screens[current_screen].col * font_info.width, row * font_info.height,
						font_info.width, font_info.height);
		}
	}
	update_cursor();
}