#include "../includes/config.h"
#include "../kernel/terminal.h"
#include "../kernel/kernel.h"
#include "../kernel/display.h"
#include "kprint/kprint.h"

static void serial_print_hex(u32_t val)
{
	char hex[] = "0123456789abcdef";
	outb(0x3F8, '0');
	outb(0x3F8, 'x');
	for (int i = 7; i >=0; i--)
		outb(0x3F8, hex[(val >> (i * 4)) & 0xF]);
	outb(0x3F8, '\n');
}

// WARNING: kputchar() does not flush is output to the terminal
// you have to do it right after
void	kputchar(char c)
{
	#if VIDEO_MODE == MODE_FRAMEBUFFER
		const u32_t rows = g_display.height / font_header.charsize;
		// serial_print_hex(g_display.row);  // valeur de row
		// serial_print_hex(rows);           // valeur max
		if (g_display.row >= rows) {
			scroll();
		}
		putchar_framebuffer(c, 0, 0);
	#else
		putchar_vga(c);
		if (g_display.row >= g_display.height)
			scroll();
	#endif
}