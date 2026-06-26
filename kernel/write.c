#include "../includes/stddef.h"
#include "kernel_internal.h"

static inline void outb(u16_t port, u8_t val)
{
	__asm__ volatile ("outb %0, %1" : : "a"(val), "Nd"(port));
}

void set_cursor(int x, int y)
{
	u16_t pos = y * 80 + x;  // 80 colonnes par ligne

	outb(0x3D4, 0x0F);
	outb(0x3D5, (u8_t)(pos & 0xFF));
	outb(0x3D4, 0x0E);
	outb(0x3D5, (u8_t)((pos >> 8) & 0xFF));
}

static void	terminal_putentryat(char c, u8_t color, size_t x, size_t y)
{
	const size_t index = y * VGA_WIDTH + x;
	terminal_buffer[index] = vga_entry(c, color);
}

void	kputchar(char c)
{
	if (c == '\n') {
		terminal_row++;
		terminal_column = 0;
	}
	else {
		terminal_putentryat(c, terminal_color, terminal_column, terminal_row);
		if (++terminal_column == VGA_WIDTH) {
			terminal_column = 0;
			if (++terminal_row == VGA_HEIGHT)
				terminal_row = 0;
		}
	}
	set_cursor(terminal_column, terminal_row);
}

void	kwrite(const void* data, size_t size)
{
	int tmp = 0;
	const char *str = data;

	for (size_t i = 0; i < size; i++) {
		if (tmp >= 15)
			tmp = 0;
		// set_term_color(vga_entry_color( VGA_COLOR_BLACK, VGA_COLOR_WHITE));
		// set_term_color(vga_entry_color(i % VGA_COLOR_END, VGA_COLOR_BLACK));
		kputchar(str[i]);
		tmp++;
	}
}