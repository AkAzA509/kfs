#include "test_support.h"
#include <core/tty.h>
#include <drivers/console.h>

t_screen g_screen;
t_screen_data g_screens[MAX_SCREENS];
int current_screen;

static char rendered[SCREEN_COLS];

void overwrite_at(u32_t col, char c)
{
	if (col < SCREEN_COLS)
		rendered[col] = c;
}

void move_cursor_to(size_t col)
{
	g_screens[current_screen].col = col;
}

u8_t get_current_col(void)
{
	return g_screens[current_screen].col;
}

int screen_putchar(char c)
{
	(void)c;
	return 1;
}

void screen_switch(int new_id)
{
	current_screen = new_id;
}

void screen_clear(t_screen_data *screen)
{
	screen->col = 0;
}

void print_prompt(void)
{
}

void shell_execute(char *input)
{
	(void)input;
}

/* Validate editor insertion, cursor movement and deletion behavior. */
void __kstart(unsigned long magic, unsigned long address)
{
	t_line_editor *editor = &g_screens[0].editor;

	(void)address;
	test_serial_init();
	test_serial_puts("TEST_START tty\n");

	if (magic != MULTIBOOT_BOOTLOADER_MAGIC) {
		test_serial_puts("TEST_FAIL invalid-multiboot-magic\n");
		test_exit(0x11);
	}

	g_screen.total_cols = 80;
	current_screen = 0;
	editor_start();
	editor_putchar('a');
	editor_putchar('c');
	move_cursor(move_one_left);
	editor_putchar('b');
	if (editor->len != 3 || editor->edit_pos != 2 ||
	    editor->buffer[0] != 'a' || editor->buffer[1] != 'b' ||
	    editor->buffer[2] != 'c') {
		test_serial_puts("TEST_FAIL tty-insert\n");
		test_exit(0x12);
	}

	editor_backspace();
	if (editor->len != 2 || editor->edit_pos != 1 ||
	    editor->buffer[0] != 'a' || editor->buffer[1] != 'c') {
		test_serial_puts("TEST_FAIL tty-backspace\n");
		test_exit(0x13);
	}

	editor_delete();
	if (editor->len != 1 || editor->buffer[0] != 'a') {
		test_serial_puts("TEST_FAIL tty-delete\n");
		test_exit(0x14);
	}

	test_serial_puts("TEST_PASS tty\n");
	test_serial_puts("TEST_RUN_COMPLETE PASS\n");
	test_exit(0x10);
}
