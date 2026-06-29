#include "../includes/stddef.h"
#include "../includes/fonts.h"
#include "kernel_internal.h"

void set_cursor(int x, int y)
{
	u16_t pos = y * MAX_COL + x;

	outb(0x3D4, 0x0F);
	outb(0x3D5, (u8_t)(pos & 0xFF));
	outb(0x3D4, 0x0E);
	outb(0x3D5, (u8_t)((pos >> 8) & 0xFF));
}

void	send_char_to_vga(char c, u8_t color, size_t x, size_t y)
{
	const size_t index = y * MAX_COL + x;
	t_buffer[index] = vga_entry(c, color);
}

void	kputchar(char c)
{
	// if (c == '\n') {
	// 	t_row++;
	// 	t_column = 0;
	// }
	// else {
	// 	send_char_to_vga(c, t_color, t_column, t_row);
	// 	if (++t_column == MAX_COL) {
	// 		t_column = 0;
	// 		if (++t_row == MAX_LINE)
	// 			t_row = 0;
	// 	}
	// }
	// set_cursor(t_column, t_row);
	
	if (c == '\n' || c == '\r') {
		newline:
			t_column = 0;
			t_row++;
		if (t_row >= MAX_LINE)
			t_row = 0;
		return;
	}

	*(t_buffer + (t_column + t_row * MAX_COL) * 2) = c & 0xFF;
	*(t_buffer + (t_column + t_row * MAX_COL) * 2 + 1) = ATTRIBUTE;

	t_column++;
	if (t_column >= MAX_COL)
		goto newline;
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