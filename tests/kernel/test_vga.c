#include "test_support.h"
#include <drivers/console.h>
#include <drivers/vga.h>

#define VGA_WIDTH 80
#define VGA_HEIGHT 25
#define VGA_CELLS (VGA_WIDTH * VGA_HEIGHT)

t_screen g_screen;
t_screen_data g_screens[MAX_SCREENS];
int current_screen;

static u16_t buffer[VGA_CELLS];

u8_t make_color(t_color foreground, t_color background)
{
	return (u8_t)(foreground | (background << 4));
}

/* Validate VGA rendering, clearing and scrolling against a memory buffer. */
void __kstart(unsigned long magic, unsigned long address)
{
	(void)address;
	test_serial_init();
	test_serial_puts("TEST_START vga\n");

	if (magic != MULTIBOOT_BOOTLOADER_MAGIC) {
		test_serial_puts("TEST_FAIL invalid-multiboot-magic\n");
		test_exit(0x11);
	}

	g_screen.buf = buffer;
	g_screen.width = VGA_WIDTH;
	g_screen.height = VGA_HEIGHT;
	g_screens[0].color = make_color(COLOR_LIGHT_GREY, COLOR_BLUE);

	if (vga_entry('A', g_screens[0].color) !=
	    ((u16_t)'A' | ((u16_t)g_screens[0].color << 8))) {
		test_serial_puts("TEST_FAIL vga-entry\n");
		test_exit(0x12);
	}

	clear_physical_vga();
	for (size_t i = 0; i < VGA_CELLS; i++) {
		if (buffer[i] != vga_entry(' ', g_screens[0].color)) {
			test_serial_puts("TEST_FAIL vga-clear\n");
			test_exit(0x13);
		}
	}

	putpixel_vga('X', 0x1f, 3, 4);
	if (buffer[4 * VGA_WIDTH + 3] != vga_entry('X', 0x1f)) {
		test_serial_puts("TEST_FAIL vga-putpixel\n");
		test_exit(0x14);
	}

	putpixel_vga('L', 0x12, 0, 0);
	putpixel_vga('R', 0x34, VGA_WIDTH - 1, VGA_HEIGHT - 1);
	if (buffer[0] != vga_entry('L', 0x12) ||
	    buffer[VGA_CELLS - 1] != vga_entry('R', 0x34)) {
		test_serial_puts("TEST_FAIL vga-corners\n");
		test_exit(0x15);
	}

	for (size_t row = 0; row < VGA_HEIGHT; row++)
		for (size_t col = 0; col < VGA_WIDTH; col++)
			buffer[row * VGA_WIDTH + col] =
				vga_entry((char)('A' + row), (u8_t)row);
	scroll_physical_vga();
	for (size_t col = 0; col < VGA_WIDTH; col++) {
		if (buffer[col] != vga_entry('B', 1) ||
		    buffer[(VGA_HEIGHT - 1) * VGA_WIDTH + col] !=
			    vga_entry(' ', g_screens[0].color)) {
			test_serial_puts("TEST_FAIL vga-scroll\n");
			test_exit(0x15);
		}
	}

	for (size_t row = 0; row < VGA_HEIGHT - 1; row++)
		for (size_t col = 0; col < VGA_WIDTH; col++)
			buffer[row * VGA_WIDTH + col] =
				vga_entry((char)('a' + row), 0x0e);
	scroll_physical_vga();
	for (size_t col = 0; col < VGA_WIDTH; col++) {
		if (buffer[col] != vga_entry('b', 0x0e)) {
			test_serial_puts("TEST_FAIL vga-second-scroll\n");
			test_exit(0x16);
		}
	}

	test_serial_puts("TEST_PASS vga\n");
	test_serial_puts("TEST_RUN_COMPLETE PASS\n");
	test_exit(0x10);
}
