#include <stdio.h>
#include <kernel/kernel.h>
#include <arch/i386/vga.h>
#include <kernel/init.h>
#include <string.h>

// pack a char with is color (fg, bg)
inline u16_t vga_entry(unsigned char uc, u8_t color)
{
	return (u16_t) uc | (u16_t) color << 8;
}

// set the vga cursor with the io port
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
	const size_t	vga_idx = y * g_screen.width + x;
	const size_t	scr_idx = y * SCREEN_COLS + x;
	const u16_t		entry = vga_entry((unsigned char)c, color);
	u16_t	*vga_mem = (u16_t *)g_screen.buf;

	vga_mem[vga_idx] = entry;

	g_screens[current_screen].text_buf[scr_idx] = c;
	g_screens[current_screen].color_buf[scr_idx] = color;
}

void	scroll_vga(void)
{
	const size_t	line_size = g_screen.width;
	const size_t	total_size = g_screen.width * g_screen.height;
	u16_t	*vga_mem = (u16_t *)g_screen.buf;

	// Move the memory up by 1 row
	memmove(vga_mem, vga_mem + line_size, (total_size - line_size) * sizeof(u16_t));

	// clear the "new" line down
	for (size_t x = 0; x < g_screen.width; ++x)
		vga_mem[(g_screen.height - 1) * g_screen.width + x] = vga_entry(' ', g_screen.color);

	// mirror the back screen
	t_screen_data *d = &g_screens[current_screen];
	for (size_t r = 0; r < g_screen.height - 1; r++) {
		for (size_t c = 0; c < g_screen.width; c++) {
			size_t	dst = r * SCREEN_COLS + c;
			size_t	src = (r + 1) * SCREEN_COLS + c;
			d->text_buf[dst] = d->text_buf[src];
			d->color_buf[dst] = d->color_buf[src];
		}
	}

	// clear the "new" line down
	for (size_t c = 0; c < g_screen.width; ++c) {
		size_t	idx = (g_screen.height - 1) * SCREEN_COLS + c;
		d->text_buf[idx] = ' ';
		d->color_buf[idx] = g_screen.color;
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

	g_screens[current_screen].col = g_screen.col;
	g_screens[current_screen].row = g_screen.row;
}

// clear the back buf who been swap with the front
void	clear_physical_vga(void)
{
	const size_t	total_size = g_screen.width * g_screen.height;
	u16_t	*vga_mem = (u16_t *)g_screen.buf;

	for (size_t i = 0; i < total_size; ++i)
		vga_mem[i] = vga_entry(' ', g_screen.color);

	g_screen.col = g_screen.row = 0;
}

// call the back buf cleen and clean the back screen
void	clear_vga(void)
{
	clear_physical_vga();

	t_screen_data	*d = &g_screens[current_screen];

	for (size_t r = 0; r < g_screen.height; r++) {
		for (size_t c = 0; c < g_screen.width; c++) {
			size_t idx = r * SCREEN_COLS + c;
			d->text_buf[idx] = ' ';
			d->color_buf[idx] = g_screen.color;
		}
	}
}