#include <arch/i386/framebuffer.h>
#include <arch/i386/console.h>
#include <kernel/shell.h>
#include <kernel/tty.h>
#include <kernel/log.h>
#include <stddef.h>
#include <string.h>

#define MAX_LINE 100

typedef struct s_line_editor {
	char	buffer[MAX_LINE];
	size_t	len;
	size_t	edit_pos;
	u8_t	input_boundary_col;
}			t_line_editor;

t_line_editor	g_line_editor;

// #define DEBUG
#ifdef DEBUG
void	debug_editor(void)
{
	klog("g_line_editor debug:\n");
	klog("\tlen      : %lu\n", g_line_editor.len);
	klog("\tedist_pos: %lu\n", g_line_editor.edit_pos);
	klog("\tinput_bnd: %u\n", g_line_editor.input_boundary_col);
}
#endif // DEBUG

static void	editor_remove_char(t_line_editor *le, size_t pos)
{
	memmove(&le->buffer[pos], &le->buffer[pos + 1], le->len - pos - 1);
	le->len--;

	for (size_t i = pos; i < le->len; i++)
		overwrite_at(le->input_boundary_col + i, le->buffer[i]);
	overwrite_at(le->input_boundary_col + le->len, ' ');
}

void	editor_backspace(void)
{
	t_line_editor *le = &g_line_editor;

	if (le->edit_pos == 0)
		return ;
	le->edit_pos--;
	#ifdef DEBUG
	// debug_editor();
	#endif // DEBUG
	editor_remove_char(le, le->edit_pos);
	move_cursor_to(le->input_boundary_col + le->edit_pos);
}

void	editor_delete(void)
{
	t_line_editor *le = &g_line_editor;

	if (le->edit_pos >= le->len)
		return ;
	editor_remove_char(le, le->edit_pos);
	move_cursor_to(le->input_boundary_col + le->edit_pos);
}

// Send a 1 to move forward
// and a -1 to move backward
// Be carefull, if the value is something other than 1
// the cursor will move accordingly
// Prefer move_cursor_to() for put the cursor at
// a specific position
void	move_cursor(i8_t direction)
{
	t_line_editor *le = &g_line_editor;

	if ((le->edit_pos == 0 && direction < 0) ||
		(le->edit_pos == g_screen.total_cols && direction > 0) ||
		direction == 0)
		return ;
	if (le->edit_pos + direction <= le->len)
		le->edit_pos += direction;
	move_cursor_to(le->input_boundary_col + le->edit_pos);
}

void	editor_start(void)
{
	print_prompt();
	g_line_editor.len = 0;
	g_line_editor.edit_pos = 0;
	g_line_editor.input_boundary_col = get_current_col();
}

void	editor_putchar(char c)
{
	if (g_line_editor.len >= MAX_LINE - 1)
		return ;
	if (c == '\n') {
		screen_putchar('\n');
		shell_execute(g_line_editor.buffer, g_line_editor.len);
		editor_start();
		return ;
	}

	if (g_line_editor.edit_pos < g_line_editor.len)
	{
		memmove(&g_line_editor.buffer[g_line_editor.edit_pos + 1],
			&g_line_editor.buffer[g_line_editor.edit_pos],
			g_line_editor.len - g_line_editor.edit_pos);
	}
	g_line_editor.buffer[g_line_editor.edit_pos] = c;
	g_line_editor.len++;
	g_line_editor.edit_pos++;

	for (size_t i = g_line_editor.edit_pos - 1; i < g_line_editor.len; i++)
		overwrite_at(g_line_editor.input_boundary_col + i, g_line_editor.buffer[i]);
	move_cursor_to(g_line_editor.input_boundary_col + g_line_editor.edit_pos);
}

// void	editor_submit(void)
// {
// }
