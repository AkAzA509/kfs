#ifndef TERMINAL_H
#define TERMINAL_H

#include <stdint.h>

typedef enum e_color t_color;

#define BOOT_LOG "[ system ] "

void	set_term_color(u8_t color);
u8_t	make_color(t_color fg, t_color bg);
u32_t	color_to_rgb(t_color color);

void	init_screens(void);
void	putchar(char c);
void	scroll(void);
void	update_cursor(void);
void	screen_switch(int new_id);
void	backspace(void);

#endif // TERMINAL_H