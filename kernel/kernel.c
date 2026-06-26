#include "../includes/stddef.h"
#include "../includes/kernel.h"
#include "kernel_internal.h"

/* Check if the compiler thinks you are targeting the wrong operating system. */
#if defined(__linux__)
#error "You are not using a cross-compiler, you will most certainly run into trouble"
#endif

/* Check for the 32-bit ix86 targets. */
#if !defined(__i386__)
#error "This code needs to be compiled with a ix86-elf compiler"
#endif

size_t	terminal_row;
size_t	terminal_column;
u8_t	terminal_color;
u16_t*	terminal_buffer = (u16_t*)VGA_MEMORY;

void init_term(void)
{
	terminal_row = 0;
	terminal_column = 0;
	terminal_color = vga_entry_color(VGA_COLOR_GREEN, VGA_COLOR_BLACK);
	
	for (size_t y = 0; y < VGA_HEIGHT; y++) {
		for (size_t x = 0; x < VGA_WIDTH; x++) {
			// terminal_color = vga_entry_color(y % VGA_COLOR_END, (x + y) % VGA_COLOR_END);
			const size_t index = y * VGA_WIDTH + x;
			terminal_buffer[index] = vga_entry(' ', terminal_color);
		}
	}
}

void kernel_main(void)
{
	init_term();
	// kprint("42");
	
	keyborad_handler();
}