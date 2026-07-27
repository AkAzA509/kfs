#ifndef CONSOLE_H
#define CONSOLE_H

#include <kernel/init.h>
#include <stdint.h>
#include <stdbool.h>
// #include <stddef.h>

typedef enum e_color t_color;

#define BOOT_LOG "[system] "

typedef enum e_color {
	COLOR_BLACK = 0,
	COLOR_BLUE = 1,
	COLOR_GREEN = 2,
	COLOR_CYAN = 3,
	COLOR_RED = 4,
	COLOR_MAGENTA = 5,
	COLOR_BROWN = 6,
	COLOR_LIGHT_GREY = 7,
	COLOR_DARK_GREY = 8,
	COLOR_LIGHT_BLUE = 9,
	COLOR_LIGHT_GREEN = 10,
	COLOR_LIGHT_CYAN = 11,
	COLOR_LIGHT_RED = 12,
	COLOR_LIGHT_MAGENTA = 13,
	COLOR_LIGHT_BROWN = 14,
	COLOR_WHITE = 15,
	COLOR_END
}	t_color;

void	set_term_color(u8_t color);
u8_t	make_color(t_color fg, t_color bg);
u32_t	color_to_rgb(t_color color);

int		screen_putchar(char c);
void	screen_switch(int new_id);
void	screen_snap(void);
void	screen_scroll(int delta);

void	move_cursor(i8_t direction);
void	move_cursor_to(size_t col);
void	overwrite_at(size_t col, char c);

u8_t	get_current_col(void);

bool	line_visible(t_screen_data *s, u32_t line);
bool	pinned_to_bottom(t_screen_data *s);

#endif // CONSOLE_H