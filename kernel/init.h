#ifndef INIT_H
#define INIT_H

#include "../includes/stdint.h"
#include "../includes/stddef.h"

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

u8_t	make_color(t_color fg, t_color bg);
u32_t	color_to_rgb(t_color color);

extern unsigned char	font_data[];
extern int				current_screen;

#define MAX_SCREENS		4
#define FB_MAX_WIDTH	800
#define FB_MAX_HEIGHT	400
#define SCREEN_COLS		80
#define SCREEN_ROWS		25

// Contenu LÉGER de chaque écran (x4, coût mémoire négligeable)
typedef struct s_screen_data {
	char	text_buf[SCREEN_COLS * SCREEN_ROWS];
	u8_t	color_buf[SCREEN_COLS * SCREEN_ROWS];
	size_t	col;
	size_t	row;
	u8_t	color;
}			t_screen_data;

// État de rendu PHYSIQUE actif (un seul, jamais x4)
typedef struct s_screen {
	void	*buf;		// adresse mémoire physique : 0xB8000 (vga) ou framebuffer_addr (fb)
	u32_t	*back_buf;	// buffer pixel intermédiaire, UN SEUL, utilisé seulement en mode fb
	size_t	col;
	size_t	row;
	int		cursor_col;
	int		cursor_row;
	u32_t	width;
	u32_t	height;
	u32_t	pitch;
	u8_t	color;
	u8_t	bpp;
	u8_t	mode;		// 0 = framebuffer, 1 = vga
}			t_screen;

typedef struct s_display_driver {
	void	(*putchar)(char c);
	void	(*scroll)(void);
	void	(*clear)(void);
}			t_display_driver;

extern t_screen				g_screen;
extern t_screen_data		g_screens[MAX_SCREENS];
extern t_display_driver		*current_driver;

typedef struct t_multiboot_info multiboot_info;

void	debug_diplay();
void	init_term(void);
void	init_display(multiboot_info *mbi);
void	screen_switch(int new_id);

#endif // INIT_H