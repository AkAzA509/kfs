#include "test_support.h"
#include <drivers/console.h>
#include <drivers/framebuffer.h>

#define WIDTH 24
#define HEIGHT 40
#define STRIDE 32

t_screen g_screen;
t_screen_data g_screens[MAX_SCREENS];
int current_screen;
t_font_info font_info = {
	.glyph_count = 256,
	.width = 8,
	.height = 16,
	.headersize = 4,
	.bytesperglyph = 16,
};
extern unsigned char font_data[];

static u32_t front[STRIDE * HEIGHT];
static u32_t back[WIDTH * HEIGHT];

u8_t make_color(t_color foreground, t_color background)
{
	return (u8_t)(foreground | (background << 4));
}

u32_t color_to_rgb(t_color color)
{
	return color == COLOR_WHITE ? 0xffffff : 0;
}

/* Validate framebuffer glyph rendering, flushing and clearing. */
void __kstart(unsigned long magic, unsigned long address)
{
	(void)address;
	test_serial_init();
	test_serial_puts("TEST_START framebuffer\n");

	if (magic != MULTIBOOT_BOOTLOADER_MAGIC) {
		test_serial_puts("TEST_FAIL invalid-multiboot-magic\n");
		test_exit(0x11);
	}

	g_screen.buf = front;
	g_screen.back_buf = back;
	g_screen.width = WIDTH;
	g_screen.height = HEIGHT;
	g_screen.pitch = STRIDE * sizeof(u32_t);
	g_screens[0].color = make_color(COLOR_WHITE, COLOR_BLACK);

	render_glyph_fb('A', g_screens[0].color, 0, 0);
	u8_t *glyph = font_data + font_info.headersize + 'A' * font_info.height;
	for (u32_t y = 0; y < font_info.height; y++) {
		for (u32_t x = 0; x < font_info.width; x++) {
			u32_t expected = (glyph[y] & (0x80 >> x)) ? 0xffffff :
								    0;
			if (back[y * WIDTH + x] != expected) {
				test_serial_puts(
					"TEST_FAIL framebuffer-glyph\n");
				test_exit(0x12);
			}
		}
	}

	for (u32_t y = 0; y < HEIGHT; y++)
		for (u32_t x = 0; x < STRIDE; x++)
			front[y * STRIDE + x] = 0xdeadbeef;
	flush_rect_fb(2, 1, 12, 10);
	for (u32_t y = 0; y < HEIGHT; y++) {
		for (u32_t x = 0; x < STRIDE; x++) {
			bool inside = x >= 2 && x < 14 && y >= 1 && y < 11;
			u32_t expected = inside ? back[y * WIDTH + x] :
						  0xdeadbeef;
			if (front[y * STRIDE + x] != expected) {
				test_serial_puts(
					"TEST_FAIL framebuffer-flush\n");
				test_exit(0x13);
			}
		}
	}
	for (size_t i = 0; i < WIDTH * HEIGHT; i++)
		back[i] = (u32_t)i;
	flush_screen_fb();
	for (u32_t y = 0; y < HEIGHT; y++)
		for (u32_t x = 0; x < WIDTH; x++)
			if (front[y * STRIDE + x] != back[y * WIDTH + x]) {
				test_serial_puts(
					"TEST_FAIL framebuffer-full-flush\n");
				test_exit(0x14);
			}

	clear_physical_fb();
	for (size_t i = 0; i < WIDTH * HEIGHT; i++) {
		if (front[(i / WIDTH) * STRIDE + i % WIDTH] != 0 ||
		    back[i] != 0) {
			test_serial_puts("TEST_FAIL framebuffer-clear\n");
			test_exit(0x15);
		}
	}
	for (u32_t y = 0; y < HEIGHT; y++)
		for (u32_t x = WIDTH; x < STRIDE; x++)
			if (front[y * STRIDE + x] != 0xdeadbeef) {
				test_serial_puts(
					"TEST_FAIL framebuffer-stride\n");
				test_exit(0x16);
			}

	font_info.glyph_count = 1;
	for (size_t i = 0; i < WIDTH * HEIGHT; i++)
		back[i] = 0x123456;
	render_glyph_fb((char)255, g_screens[0].color, 1, 1);
	u8_t *fallback = font_data + font_info.headersize;
	for (u32_t y = 0; y < font_info.height; y++)
		for (u32_t x = 0; x < font_info.width; x++) {
			u32_t expected =
				(fallback[y] & (0x80 >> x)) ? 0xffffff : 0;
			if (back[(1 * font_info.height + y) * WIDTH +
				 1 * font_info.width + x] != expected) {
				test_serial_puts(
					"TEST_FAIL framebuffer-fallback\n");
				test_exit(0x17);
			}
		}
	font_info.glyph_count = 256;

	for (u32_t y = 0; y < HEIGHT; y++)
		for (u32_t x = 0; x < WIDTH; x++)
			back[y * WIDTH + x] = y;
	scroll_physical_fb();
	for (u32_t y = 0; y < HEIGHT; y++)
		for (u32_t x = 0; x < WIDTH; x++) {
			u32_t expected = y + font_info.height < HEIGHT ?
						 y + font_info.height :
						 0;
			if (back[y * WIDTH + x] != expected ||
			    front[y * STRIDE + x] != expected) {
				test_serial_puts(
					"TEST_FAIL framebuffer-scroll\n");
				test_exit(0x18);
			}
		}

	for (size_t i = 0; i < STRIDE * HEIGHT; i++)
		front[i] = 0;
	for (size_t i = 0; i < WIDTH * HEIGHT; i++)
		back[i] = 0x123456;
	g_screen.cursor_col = -1;
	g_screen.cursor_row = -1;
	g_screens[0].col = 1;
	g_screens[0].head = 0;
	g_screens[0].view_offset = 0;
	update_cursor_fb();
	for (u32_t y = 0; y < HEIGHT; y++)
		for (u32_t x = 0; x < STRIDE; x++) {
			bool cursor = x >= 8 && x < 16 && y >= 14 && y < 16;
			u32_t expected = cursor ? 0xffffff : 0;
			if (front[y * STRIDE + x] != expected) {
				test_serial_puts(
					"TEST_FAIL framebuffer-cursor\n");
				test_exit(0x19);
			}
		}
	g_screens[0].col = 2;
	update_cursor_fb();
	for (u32_t x = 8; x < 16; x++)
		if (front[14 * STRIDE + x] != 0x123456) {
			test_serial_puts(
				"TEST_FAIL framebuffer-cursor-restore\n");
			test_exit(0x1a);
		}
	g_screens[0].col = WIDTH;
	update_cursor_fb();
	if (g_screen.cursor_col != -1 || g_screen.cursor_row != -1) {
		test_serial_puts("TEST_FAIL framebuffer-cursor-boundary\n");
		test_exit(0x1b);
	}

	test_serial_puts("TEST_PASS framebuffer\n");
	test_serial_puts("TEST_RUN_COMPLETE PASS\n");
	test_exit(0x10);
}
