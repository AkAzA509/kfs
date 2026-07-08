#include "drivers/framebuffer.h"
#include "drivers/vga.h"
#include "terminal.h"
#include "init.h"

// ===== public API , dispatch via vtable ===== //

void set_term_color(u8_t color)
{
	g_screen.color = color;
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
	if (g_screen.mode == 0) {
		u8_t bg_color = g_screen.color >> 4;
		draw_cursor(g_screen.col, g_screen.row, ~bg_color);
	}
	else
		set_cursor(g_screen.col, g_screen.row);
}

void	putchar(char c)
{
	current_driver->putchar(c);
	update_cursor();
}

void	scroll(void)
{
	current_driver->scroll();
	update_cursor();
}

void	screen_switch(int new_id)
{
	if (new_id < 0 || new_id >= MAX_SCREENS || new_id == current_screen)
		return ;

	t_screen_data *d = &g_screens[new_id];

	current_screen = new_id;
	if (g_screen.mode == 1)
		clear_physical_vga();
	else
		clear_physical_fb();

	size_t active_cols, active_rows;
	if (g_screen.mode == 1) {				// VGA
		active_cols = g_screen.width;
		active_rows = g_screen.height;
	}
	else {
		active_cols = g_screen.width / 8;
		active_rows = g_screen.height / font_header.charsize;
	}

	for (size_t r = 0; r < active_rows; r++) {
		for (size_t c = 0; c < active_cols; c++) {
			size_t idx = r * SCREEN_COLS + c;
			if (d->text_buf[idx] == '\0')
				continue ;
			if (g_screen.mode == 0)
				putpixel_fb(d->text_buf[idx], d->color_buf[idx], c, r);
			else
				putpixel_vga(d->text_buf[idx], d->color_buf[idx], c, r);
		}
	}

	g_screen.col = d->col;
	g_screen.row = d->row;
	g_screen.color = d->color;
	update_cursor();
}

void	backspace(void)
{
	if (g_screen.col == 0 && g_screen.row == 0)
		return ;

	if (g_screen.col == 0) {
		g_screen.row--;
		g_screen.col = (g_screen.mode == 1 ? g_screen.width : g_screen.width / 8) - 1;
	}
	else {
		g_screen.col--;
	}

	if (g_screen.mode == 0) {
		putpixel_fb(' ', g_screen.color, g_screen.col, g_screen.row);
		update_screens(' ');
	}
	else
		putpixel_vga(' ', g_screen.color, g_screen.col, g_screen.row);

	g_screens[current_screen].col = g_screen.col;
	g_screens[current_screen].row = g_screen.row;
	update_cursor();
}