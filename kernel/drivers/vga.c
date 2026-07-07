#include "../../helpers/helpers.h"
#include "../kernel.h"
#include "../init.h"
#include "vga.h"

inline u16_t vga_entry(unsigned char uc, u8_t color)
{
	return (u16_t) uc | (u16_t) color << 8;
}

void set_cursor(int x, int y)
{
	u16_t pos = y * g_screen.width + x;

	outb(0x3D4, 0x0F);
	outb(0x3D5, (u8_t)(pos & 0xFF));
	outb(0x3D4, 0x0E);
	outb(0x3D5, (u8_t)((pos >> 8) & 0xFF));
}

void	putpixel_vga(char c, u8_t color, size_t x, size_t y)
{
	const size_t index = y * g_screen.width + x;
	u16_t *vga_mem = (u16_t *)g_screen.buf;
	const u16_t entry = vga_entry((unsigned char)c, color);

	vga_mem[index] = entry;

	// on garde une trace "logique" dans l'écran courant, pour le switch plus tard
	g_screens[current_screen].text_buf[index] = c;
	g_screens[current_screen].color_buf[index] = color;
}

void	scroll_vga(void)
{
	u16_t *vga_mem = (u16_t *)g_screen.buf;
	const size_t line_size = g_screen.width;
	const size_t total_size = g_screen.width * g_screen.height;

	memmove(vga_mem, vga_mem + line_size, (total_size - line_size) * sizeof(u16_t));
	for (size_t x = 0; x < g_screen.width; ++x)
		vga_mem[(g_screen.height - 1) * g_screen.width + x] = vga_entry(' ', g_screen.color);

	// on répercute le scroll dans les données logiques de l'écran courant aussi
	t_screen_data *d = &g_screens[current_screen];
	memmove(d->text_buf, d->text_buf + line_size, (total_size - line_size) * sizeof(char));
	memmove(d->color_buf, d->color_buf + line_size, (total_size - line_size) * sizeof(u8_t));
	for (size_t x = 0; x < g_screen.width; ++x) {
		d->text_buf[(g_screen.height - 1) * g_screen.width + x] = ' ';
		d->color_buf[(g_screen.height - 1) * g_screen.width + x] = g_screen.color;
	}

	g_screen.row = g_screen.height - 1;
}

void putchar_vga(char c)
{
	if (c == '\n') {
		g_screen.row++;
		g_screen.col = 0;
	}
	else if (c == '\t')
		g_screen.col = (g_screen.col + 8) & ~7U;
	else {
		putpixel_vga(c, g_screen.color, g_screen.col, g_screen.row);
		if (++g_screen.col == g_screen.width) {
			g_screen.col = 0;
			g_screen.row++;
		}
	}
	if (g_screen.row >= g_screen.height)
		scroll_vga();

	// synchronise la position dans les données logiques de l'écran courant
	g_screens[current_screen].col = g_screen.col;
	g_screens[current_screen].row = g_screen.row;
}

void	clear_vga(void)
{
	u16_t *vga_mem = (u16_t *)g_screen.buf;
	const size_t total_size = g_screen.width * g_screen.height;

	for (size_t i = 0; i < total_size; ++i)
		vga_mem[i] = vga_entry(' ', g_screen.color);

	t_screen_data *d = &g_screens[current_screen];
	for (size_t i = 0; i < total_size; ++i) {
		d->text_buf[i] = ' ';
		d->color_buf[i] = g_screen.color;
	}
	g_screen.col = 0;
	g_screen.row = 0;
}