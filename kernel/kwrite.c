#include "../includes/stddef.h"
#include "kernel_internal.h"

// duplicate function into drivers/keyboard.c need to factorize the code
// write 1 byte in the port
static inline void outb(u16_t port, u8_t val)
{
	// outb = write into a hardware port
	// place the input 'a' into val, send to 'Nd'=port
	__asm__ volatile ("outb %0, %1" : : "a"(val), "Nd"(port));
}

void set_cursor(int x, int y)
{
	u16_t pos = y * VGA_WIDTH + x;

	outb(0x3D4, 0x0F);
	outb(0x3D5, (u8_t)(pos & 0xFF));
	outb(0x3D4, 0x0E);
	outb(0x3D5, (u8_t)((pos >> 8) & 0xFF));
}

void	send_char_to_vga(char c, u8_t color, size_t x, size_t y)
{
	const size_t index = y * VGA_WIDTH + x;
	t_buffer[index] = vga_entry(c, color);
}

void	kputchar(char c)
{
	if (c == '\n') {
		t_row++;
		t_column = 0;
	}
	else {
		send_char_to_vga(c, t_color, t_column, t_row);
		if (++t_column == VGA_WIDTH) {
			t_column = 0;
			if (++t_row == VGA_HEIGHT)
				t_row = 0;
		}
	}
	set_cursor(t_column, t_row);
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