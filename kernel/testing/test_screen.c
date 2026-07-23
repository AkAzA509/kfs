#include <arch/i386/framebuffer.h>
#include <arch/i386/tty.h>
#include <testing/testing.h>
#include <kernel/init.h>
#include <stdio.h>

void test_screen()
{
	printf("Test screen 0\n\n");

	screen_switch(1);
	char buffer[100] = "012345678910111213141516171819202122232425\0";
	printf("test screen 1 %s\n\n", buffer);

	screen_switch(2);
	printf("Test backspace screen 3\n il ne dois rien y avaoir apres ca :%s",
		buffer);
	for (size_t i = 0; buffer[i]; ++i) {
		backspace();
	}
	screen_switch(3);
	debug_screen();
	printf("coucou after display\n\n\n\n\n\n\nplus bas");
	backspace();
	backspace();
	backspace();
	backspace();
	backspace();
	backspace();
	backspace();
	backspace();

	screen_switch(0);
	const u32_t cols = g_screen.width / 8;
	const u32_t rows = g_screen.height / font_info.height;
	set_term_color(make_color(COLOR_CYAN, COLOR_WHITE));
	for (size_t i = 0; i < cols * rows - 1; ++i)
		printf("0");
	set_term_color(make_color(COLOR_WHITE, COLOR_BLACK));
}