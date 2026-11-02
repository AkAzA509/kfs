#include "test_support.h"
#include <drivers/console.h>
#include <drivers/framebuffer.h>

#define WIDTH 4
#define HEIGHT 2

t_screen g_screen;
t_screen_data g_screens[MAX_SCREENS];
int current_screen;
t_font_info font_info = {
	.width = 8,
	.height = 16,
};

static u16_t vga_buffer[WIDTH * HEIGHT];
static unsigned int clear_calls;
static unsigned int flush_calls;
static unsigned int scroll_calls;
static unsigned int cursor_calls;

static void display_putchar(char c, u8_t color, size_t x, size_t y)
{
	if (x < WIDTH && y < HEIGHT)
		vga_buffer[y * WIDTH + x] = (u16_t)(unsigned char)c |
					    (u16_t)color << 8;
}

static void display_scroll(void)
{
	scroll_calls++;
}

static void display_clear(void)
{
	clear_calls++;
}

static void display_flush(void)
{
	flush_calls++;
}

static void display_flush_partial(u32_t x, u32_t y, u32_t w, u32_t h)
{
	(void)x;
	(void)y;
	(void)w;
	(void)h;
	flush_calls++;
}

static void display_cursor(void)
{
	cursor_calls++;
}

static char text_at(u32_t line, u32_t column)
{
	return g_screens[0]
		.text_buf[(line % SCROLLBACK_LINES) * SCREEN_COLS + column];
}

static u8_t color_at(u32_t line, u32_t column)
{
	return g_screens[0]
		.color_buf[(line % SCROLLBACK_LINES) * SCREEN_COLS + column];
}

static void fail(const char *message, u8_t code)
{
	test_serial_puts(message);
	test_exit(code);
}

/* Validate scrollback storage, viewport movement and console redraws. */
void __kstart(unsigned long magic, unsigned long address)
{
	(void)address;
	test_serial_init();
	test_serial_puts("TEST_START console\n");

	if (magic != MULTIBOOT_BOOTLOADER_MAGIC)
		fail("TEST_FAIL invalid-multiboot-magic\n", 0x11);

	g_screen.width = WIDTH;
	g_screen.height = HEIGHT;
	g_screen.total_cols = WIDTH;
	g_screen.total_rows = HEIGHT;
	g_screen.mode = 1;
	g_screen.buf = vga_buffer;
	g_screens[0].color = make_color(COLOR_LIGHT_GREY, COLOR_BLUE);
	g_screen.display.putchar_at = display_putchar;
	g_screen.display.scroll = display_scroll;
	g_screen.display.clear = display_clear;
	g_screen.display.flush_screen = display_flush;
	g_screen.display.flush_partial = display_flush_partial;
	g_screen.display.cursor_update = display_cursor;
	current_screen = 0;

	screen_clear(&g_screens[0]);
	if (clear_calls != 1 || g_screens[0].head != 0 ||
	    g_screens[0].view_offset != 0 || g_screens[0].col != 0)
		fail("TEST_FAIL console-clear\n", 0x12);

	screen_puts("abcdefgh", 8);
	if (g_screens[0].head != 2 || g_screens[0].view_offset != 1 ||
	    g_screens[0].col != 0 || text_at(0, 0) != 'a' ||
	    text_at(1, 0) != 'e') {
		fail("TEST_FAIL console-wrap\n", 0x13);
	}
	if (color_at(0, 0) != g_screens[0].color ||
	    color_at(1, 3) != g_screens[0].color) {
		fail("TEST_FAIL console-colors\n", 0x14);
	}
	if (scroll_calls != 1)
		fail("TEST_FAIL console-scroll\n", 0x15);

	screen_scroll(-1, &g_screens[0]);
	if (g_screens[0].view_offset != 0)
		fail("TEST_FAIL console-scroll-up\n", 0x16);

	screen_scroll(-1, &g_screens[0]);
	if (g_screens[0].view_offset != 0)
		fail("TEST_FAIL console-scroll-boundary\n", 0x17);

	screen_snap(&g_screens[0]);
	if (g_screens[0].view_offset != 1 || flush_calls == 0 ||
	    cursor_calls == 0)
		fail("TEST_FAIL console-snap\n", 0x18);

	g_screens[1].color = make_color(COLOR_GREEN, COLOR_BLACK);
	screen_fputs("xy", 1, 2);
	if (g_screens[1].head != 0 || g_screens[1].col != 2 ||
	    g_screens[1].text_buf[0] != 'x' ||
	    g_screens[1].text_buf[1] != 'y' ||
	    g_screens[1].color_buf[0] != g_screens[1].color) {
		fail("TEST_FAIL console-secondary-screen\n", 0x19);
	}
	if (current_screen != 0)
		fail("TEST_FAIL console-fputs-current-screen\n", 0x1a);

	screen_switch(1);
	screen_putchar('z');
	if (current_screen != 1 || g_screens[1].col != 3 ||
	    g_screens[1].text_buf[2] != 'z') {
		fail("TEST_FAIL console-switch\n", 0x1b);
	}
	screen_switch(0);

	screen_clear(&g_screens[0]);
	for (u32_t line = 0; line < SCROLLBACK_LINES + 3; line++)
		screen_puts("abc\n", 4);
	if (g_screens[0].head != SCROLLBACK_LINES + 3 ||
	    g_screens[0].view_offset != SCROLLBACK_LINES + 2 ||
	    text_at(0, 0) != 'a' || color_at(0, 0) != g_screens[0].color) {
		fail("TEST_FAIL console-scrollback-wrap\n", 0x1c);
	}

	screen_scroll(-10000, &g_screens[0]);
	if (g_screens[0].view_offset != 4)
		fail("TEST_FAIL console-scrollback-minimum\n", 0x1d);
	screen_scroll(10000, &g_screens[0]);
	if (g_screens[0].view_offset != SCROLLBACK_LINES + 2)
		fail("TEST_FAIL console-scrollback-maximum\n", 0x1e);
	screen_snap(&g_screens[0]);
	if (g_screens[0].view_offset != SCROLLBACK_LINES + 2)
		fail("TEST_FAIL console-scrollback-snap\n", 0x1f);

	test_serial_puts("TEST_PASS console\n");
	test_serial_puts("TEST_RUN_COMPLETE PASS\n");
	test_exit(0x10);
}
