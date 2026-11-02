#include "test_support.h"
#include <drivers/console.h>
#include <drivers/framebuffer.h>

#define WIDTH 16
#define HEIGHT 32

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

static u32_t front[WIDTH * HEIGHT];
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
	g_screen.pitch = WIDTH * sizeof(u32_t);
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

	flush_rect_fb(0, 0, 8, 8);
	if (front[0] != back[0] || front[7] != back[7 * WIDTH + 7]) {
		test_serial_puts("TEST_FAIL framebuffer-flush\n");
		test_exit(0x13);
	}

	clear_physical_fb();
	for (size_t i = 0; i < WIDTH * HEIGHT; i++) {
		if (front[i] != 0 || back[i] != 0) {
			test_serial_puts("TEST_FAIL framebuffer-clear\n");
			test_exit(0x14);
		}
	}

	test_serial_puts("TEST_PASS framebuffer\n");
	test_serial_puts("TEST_RUN_COMPLETE PASS\n");
	test_exit(0x10);
}
