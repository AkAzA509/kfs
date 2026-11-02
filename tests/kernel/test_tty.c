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
	if (!g_screens[0].editor.prompt_displayed ||
	    g_screens[0].editor.input_boundary_col != 0) {
		test_serial_puts("TEST_FAIL tty-start\n");
		test_exit(0x12);
	}
	editor_putchar('a');
	editor_putchar('c');
	move_cursor(move_one_left);
	editor_putchar('b');
	if (editor->len != 3 || editor->edit_pos != 2 ||
	    editor->buffer[0] != 'a' || editor->buffer[1] != 'b' ||
	    editor->buffer[2] != 'c') {
		test_serial_puts("TEST_FAIL tty-insert\n");
		test_exit(0x13);
	}

	move_cursor(move_start);
	if (editor->edit_pos != 0) {
		test_serial_puts("TEST_FAIL tty-move-start\n");
		test_exit(0x14);
	}
	move_cursor(move_one_left);
	if (editor->edit_pos != 0) {
		test_serial_puts("TEST_FAIL tty-left-boundary\n");
		test_exit(0x15);
	}
	move_cursor(move_end);
	if (editor->edit_pos != editor->len) {
		test_serial_puts("TEST_FAIL tty-move-end\n");
		test_exit(0x16);
	}
	move_cursor(move_one_right);
	if (editor->edit_pos != editor->len) {
		test_serial_puts("TEST_FAIL tty-right-boundary\n");
		test_exit(0x17);
	}
	move_cursor(move_start);
	editor_putchar('\t');
	if (editor->len != 11 || editor->edit_pos != 8) {
		test_serial_puts("TEST_FAIL tty-tab\n");
		test_exit(0x18);
	}
	for (size_t i = 0; i < 8; i++) {
		if (editor->buffer[i] != ' ') {
			test_serial_puts("TEST_FAIL tty-tab-content\n");
			test_exit(0x19);
		}
	}
	if (editor->buffer[8] != 'a' || editor->buffer[9] != 'b' ||
	    editor->buffer[10] != 'c') {
		test_serial_puts("TEST_FAIL tty-tab-shift\n");
		test_exit(0x1a);
	}
	editor_start();
	editor_putchar('a');
	editor_putchar('c');
	move_cursor(move_one_left);
	editor_putchar('b');
	editor_backspace();
	if (editor->len != 2 || editor->edit_pos != 1 ||
	    editor->buffer[0] != 'a' || editor->buffer[1] != 'c') {
		test_serial_puts("TEST_FAIL tty-backspace\n");
		test_exit(0x1b);
	}

	editor->edit_pos = 1;
	editor_delete();
	if (editor->len != 1 || editor->buffer[0] != 'a') {
		test_serial_puts("TEST_FAIL tty-delete\n");
		test_exit(0x1c);
	}
	editor->edit_pos = 0;
	editor_backspace();
	if (editor->len != 1 || editor->edit_pos != 0) {
		test_serial_puts("TEST_FAIL tty-backspace-boundary\n");
		test_exit(0x1d);
	}
	editor->edit_pos = editor->len;
	editor_delete();
	if (editor->len != 1 || editor->edit_pos != 1) {
		test_serial_puts("TEST_FAIL tty-delete-boundary\n");
		test_exit(0x1e);
	}

	g_screen.total_cols = 16;
	move_cursor_to(3);
	editor_start();
	editor_putchar('\t');
	if (editor->len != 5 || editor->edit_pos != 5 ||
	    editor->input_boundary_col != 3) {
		test_serial_puts("TEST_FAIL tty-tab-offset\n");
		test_exit(0x1f);
	}
	for (size_t i = 0; i < 5; i++)
		if (editor->buffer[i] != ' ') {
			test_serial_puts("TEST_FAIL tty-tab-offset-content\n");
			test_exit(0x20);
		}

	g_screen.total_cols = 8;
	move_cursor_to(0);
	editor_start();
	for (size_t i = 0; i < 8; i++)
		editor_putchar((char)('0' + i));
	editor_putchar('x');
	if (editor->len != 8 || editor->edit_pos != 8 ||
	    editor->buffer[7] != '7') {
		test_serial_puts("TEST_FAIL tty-capacity\n");
		test_exit(0x21);
	}

	g_screen.total_cols = 80;
	current_screen = 0;
	move_cursor_to(0);
	editor_start();
	editor_putchar('0');
	g_screens[1].col = 2;
	current_screen = 0;
	editor_switch(1);
	if (current_screen != 1 || !g_screens[1].editor.prompt_displayed ||
	    g_screens[1].editor.input_boundary_col != 2) {
		test_serial_puts("TEST_FAIL tty-screen-switch\n");
		test_exit(0x22);
	}
	editor_putchar('1');
	editor_switch(0);
	if (current_screen != 0 || editor->len != 1 ||
	    editor->buffer[0] != '0') {
		test_serial_puts("TEST_FAIL tty-screen-isolation\n");
		test_exit(0x23);
	}

	test_serial_puts("TEST_PASS tty\n");
	test_serial_puts("TEST_RUN_COMPLETE PASS\n");
	test_exit(0x10);
}
