#include <drivers/framebuffer.h>
#include <drivers/console.h>
#include <kernel/shell.h>
#include <kernel/tty.h>
#include <kernel/log.h>
#include <stddef.h>
#include <string.h>

// #define DEBUG
#ifdef DEBUG
void debug_editor(void)
{
	t_line_editor *le = &g_screens[current_screen].editor;
	klog("line_editor debug:\n");
	klog("\tlen      : %lu\n", le->len);
	klog("\tedist_pos: %lu\n", le->edit_pos);
	klog("\tinput_bnd: %u\n", le->input_boundary_col);
}
#endif // DEBUG

static void editor_remove_char(t_line_editor *le, size_t pos)
{
	memmove(&le->buffer[pos], &le->buffer[pos + 1], le->len - pos - 1);
	le->len--;

	for (size_t i = pos; i < le->len; i++)
		overwrite_at(le->input_boundary_col + i, le->buffer[i]);
	overwrite_at(le->input_boundary_col + le->len, ' ');
}

void editor_backspace(void)
{
	t_line_editor *le = &g_screens[current_screen].editor;

	if (le->edit_pos == 0)
		return;
	le->edit_pos--;
#ifdef DEBUG
// debug_editor();
#endif // DEBUG
	editor_remove_char(le, le->edit_pos);
	move_cursor_to(le->input_boundary_col + le->edit_pos);
}

void editor_delete(void)
{
	t_line_editor *le = &g_screens[current_screen].editor;

	if (le->edit_pos >= le->len)
		return;
	editor_remove_char(le, le->edit_pos);
}

void move_cursor(e_editor_move editor_move)
{
	t_line_editor *le = &g_screens[current_screen].editor;

	if ((le->edit_pos == 0 && editor_move < 0) ||
	    (le->edit_pos == g_screen.total_cols && editor_move > 0))
		return;

	if ((editor_move == move_one_left || editor_move == move_one_right) &&
	    le->edit_pos + editor_move <= le->len)
		le->edit_pos += editor_move;
	else if (editor_move == move_end && le->edit_pos != le->len)
		le->edit_pos = le->len;
	else if (editor_move == move_start && le->edit_pos > 0)
		le->edit_pos = 0;
	else
		return;
	move_cursor_to(le->input_boundary_col + le->edit_pos);
}

void editor_start(void)
{
	print_prompt();
	g_screens[current_screen].editor.len = 0;
	g_screens[current_screen].editor.edit_pos = 0;
	g_screens[current_screen].editor.input_boundary_col = get_current_col();
}

static void handle_tab(void)
{
	t_line_editor *le = &g_screens[current_screen].editor;
	size_t current_col = le->input_boundary_col + le->edit_pos;
	size_t next_stop = (current_col + 8) & ~7U;
	size_t nb_spaces = next_stop - current_col;

	for (size_t i = 0; i < nb_spaces; i++)
		editor_putchar(' ');
}

void editor_putchar(char c)
{
	t_line_editor *le = &g_screens[current_screen].editor;
	if (le->len >= MAX_LINE - 1)
		return;
	if (c == '\n') {
		screen_putchar(c);
		shell_execute(le->buffer, le->len);
		editor_start();
		return;
	}
	if (c == '\t') {
		handle_tab();
		return;
	}

	if (le->edit_pos < le->len)
		memmove(&le->buffer[le->edit_pos + 1],
			&le->buffer[le->edit_pos], le->len - le->edit_pos);

	le->buffer[le->edit_pos] = c;
	le->len++;
	le->edit_pos++;

	for (size_t i = le->edit_pos - 1; i < le->len; i++)
		overwrite_at(le->input_boundary_col + i, le->buffer[i]);
	move_cursor_to(le->input_boundary_col + le->edit_pos);
}