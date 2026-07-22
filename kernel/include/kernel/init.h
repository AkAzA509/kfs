#ifndef INIT_H
#define INIT_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

extern unsigned char	font_data[];
extern int				current_screen;

#define MAX_SCREENS		4
#define FB_MAX_WIDTH	1680
#define FB_MAX_HEIGHT	1000
#define SCREEN_COLS		210
#define SCREEN_ROWS		63

// multiscreen struct
// save the state of each screen (content, position, color)
typedef struct s_screen_data {
	char	text_buf[SCREEN_COLS * SCREEN_ROWS];
	u8_t	color_buf[SCREEN_COLS * SCREEN_ROWS];
	size_t	col;
	size_t	row;
	u8_t	color;
}			t_screen_data;

// screen struct
// contain all display and screen info, the back buffer ...
typedef struct s_screen {
	void	*buf;
	u32_t	*back_buf;
	u16_t	col;
	u16_t	row;
	u16_t	total_rows;
	u16_t	total_cols;
	int		cursor_col;
	int		cursor_row;
	u32_t	width;
	u32_t	height;
	u32_t	pitch;
	u8_t	color;
	u8_t	bpp;
	u8_t	mode;		// 0 = framebuffer, 1 = vga
}			t_screen;

// vtable: struct of pointer to function (methode for the display management)
typedef struct s_display_driver {
	void	(*putchar)(char c);
	void	(*scroll)(void);
	void	(*clear)(void);
}			t_display_driver;

extern t_screen				g_screen;
extern t_screen_data		g_screens[MAX_SCREENS];
extern t_display_driver		*current_driver;

typedef struct t_multiboot_info multiboot_info;

void	debug_screen();
bool	init_term(void);
void	init_display(multiboot_info *mbi);

#endif // INIT_H