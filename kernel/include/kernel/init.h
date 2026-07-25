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
#define SCROLLBACK_LINES 1024

// multiscreen struct
// save the state of each screen (content, position, color)
// and the rendering view [historic [screen view] historic]
typedef struct s_screen_data {
	char	text_buf[SCREEN_COLS * SCROLLBACK_LINES];
	u8_t	color_buf[SCREEN_COLS * SCROLLBACK_LINES];
	size_t	col;			// write cursor column on the "head" line
	u32_t	head;			// logical line index currently being written to, monotonically increasing, never decremented
	u32_t	view_offset;	// logical line index rendered at the top of the screen, independent from head,
							// only changed by manual scroll or explicit snap-to-bottom
	u8_t	color;
}			t_screen_data;

// screen struct
// contain all display and screen info, the back buffer ...
typedef struct s_screen {
	void	*buf;
	u32_t	*back_buf;
	u32_t	width;
	u32_t	height;
	u32_t	pitch;
	int		cursor_col;
	int		cursor_row;
	u16_t	total_rows;
	u16_t	total_cols;
	u8_t	bpp;
	u8_t	mode;		// 0 = framebuffer, 1 = vga
}			t_screen;

// vtable: struct of pointer to function (methode for the display management)
typedef struct s_display_driver {
	void	(*putchar_at)(char c, u8_t color, size_t x, size_t y);
	void	(*scroll)(void);
	void	(*clear)(void);
	void	(*flush_screen)(void);
	void	(*flush_partial)(u32_t x, u32_t y, u32_t w, u32_t h);
	void	(*cursor_update)(void);
}			t_display_driver;

extern t_screen				g_screen;
extern t_screen_data		g_screens[MAX_SCREENS];
extern t_display_driver		*display_d;

void	debug_screen(void);
void	debug_font(void);
void	debug_current_screen(void);

typedef struct t_multiboot_info multiboot_info;

void	init_gdt(void);
bool	init_term(void);
void	init_display(multiboot_info *mbi);

#endif // INIT_H