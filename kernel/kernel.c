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

size_t	t_row;
size_t	t_column;
u8_t	t_color;
u16_t*	t_buffer = (u16_t*)VGA_MEMORY;

void init_term(void)
{
	t_row = 0;
	t_column = 0;
	t_color = vga_entry_color(VGA_COLOR_GREEN, VGA_COLOR_BLACK);
	
	for (size_t y = 0; y < VGA_HEIGHT; y++) {
		for (size_t x = 0; x < VGA_WIDTH; x++) {
			// t_color = vga_entry_color(y % VGA_COLOR_END, (x + y) % VGA_COLOR_END);
			// const size_t index = y * VGA_WIDTH + x;
			// t_buffer[index] = vga_entry(' ', t_color);
			send_char_to_vga(' ', t_color, x, y);
		}
	}
}

void kernel_main(void)
{
	init_term();
	// kprint("42");
	
	keyboard_handler();
}