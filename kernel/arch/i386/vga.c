#include <arch/i386/console.h>
#include <arch/i386/vga.h>
#include <kernel/io.h>
#include <string.h>

// pack a char with is color (fg, bg)
inline u16_t vga_entry(unsigned char uc, u8_t color)
{
	return (u16_t) uc | (u16_t) color << 8;
}

// set the vga cursor with the io port
static void	set_cursor(int x, int y)
{
	u16_t pos = y * g_screen.width + x;

	outb(0x3D4, 0x0F);
	outb(0x3D5, (u8_t)(pos & 0xFF));
	outb(0x3D4, 0x0E);
	outb(0x3D5, (u8_t)((pos >> 8) & 0xFF));
}

void	update_cursor_vga(void)
{
	t_screen_data	*s = &g_screens[current_screen];
	int	col = s->col;
	int	row = s->head - s->view_offset;

	set_cursor(col, row);
}

void	putpixel_vga(char c, u8_t color, size_t x, size_t y)
{
	const size_t	vga_idx = y * g_screen.width + x;
	const u16_t		entry = vga_entry((unsigned char)c, color);
	u16_t	*vga_mem = (u16_t *)g_screen.buf;

	vga_mem[vga_idx] = entry;
}

void	scroll_physical_vga(void)
{
	u16_t *vga = (u16_t *)g_screen.buf;
	const size_t line = g_screen.width;
	const size_t total = g_screen.width * g_screen.height;

	memmove(vga, vga + line, (total - line) * sizeof(u16_t));
	for (size_t c = 0; c < g_screen.width; ++c)
		vga[(g_screen.height - 1) * g_screen.width + c] = vga_entry(' ', g_screens[current_screen].color);
}

// clear the back buf who been swap with the front
void	clear_physical_vga(void)
{
	const size_t	total_size = g_screen.width * g_screen.height;
	u16_t	*vga_mem = (u16_t *)g_screen.buf;

	for (size_t i = 0; i < total_size; ++i)
		vga_mem[i] = vga_entry(' ', g_screens[current_screen].color);
}


void	flush_rect_vga(u32_t x, u32_t y, u32_t w, u32_t h)
{
	(void)x;
	(void)y;
	(void)w;
	(void)h;
}

void	flush_screen_vga() {}