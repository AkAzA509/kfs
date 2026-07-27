#ifndef TERMINAL_H
#define TERMINAL_H

#include "kernel/init.h"
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

// command list for the tty
// - reboot
// - halt
// - print logo
// - print stack
// - shutdown
// - (change color)

void	set_term_color(u8_t color);
u8_t	make_color(t_color fg, t_color bg);
u32_t	color_to_rgb(t_color color);

int		kputchar(char c);
void	screen_switch(int new_id);
void	screen_snap(void);
void	screen_scroll(int delta);

typedef struct s_screen_data t_screen_data;
bool	pinned_to_bottom(t_screen_data *s);

void	backspace(void);
#endif // TERMINAL_H