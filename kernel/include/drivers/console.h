#ifndef CONSOLE_H
#define CONSOLE_H

#include <kernel/init.h>
// #include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

// typedef enum e_color t_color;

#define BOOT_LOG "[system] "
#define BOOT_TEST "[ ] "
#define BOOT_OK "[x] "

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
} t_color;

extern unsigned char font_data[];
extern int current_screen;

#define MAX_SCREENS 4
#define FB_MAX_WIDTH 1680
#define FB_MAX_HEIGHT 1000
#define SCREEN_COLS 210
#define SCREEN_ROWS 63
#define SCROLLBACK_LINES 1024

#define MAX_LINE 100

typedef struct s_line_editor {
	char buffer[MAX_LINE];
	size_t len;
	size_t edit_pos;
	u8_t input_boundary_col;
} t_line_editor;

// multiscreen struct
// save the state of each screen (content, position, color)
// and the rendering view [historic [screen view] historic]
typedef struct s_screen_data {
	char text_buf[SCREEN_COLS * SCROLLBACK_LINES];
	u8_t color_buf[SCREEN_COLS * SCROLLBACK_LINES];
	size_t col; // write cursor column on the "head" line
	u32_t head; // logical line index currently being written to, monotonically increasing, never decremented
	u32_t view_offset; // logical line index rendered at the top of the screen, independent from head,
	// only changed by manual scroll or explicit snap-to-bottom
	u8_t color;
	t_line_editor editor;
} t_screen_data;

// screen struct
// contain all display and screen info, the back buffer ...
typedef struct s_screen {
	void *buf;
	u32_t *back_buf;
	u32_t width;
	u32_t height;
	u32_t pitch;
	int cursor_col;
	int cursor_row;
	u16_t total_rows;
	u16_t total_cols;
	u8_t bpp;
	u8_t mode; // 0 = framebuffer, 1 = vga
} t_screen;

// vtable: struct of pointer to function (methode for the display management)
typedef struct s_display_driver {
	void (*putchar_at)(char c, u8_t color, size_t x, size_t y);
	void (*scroll)(void);
	void (*clear)(void);
	void (*flush_screen)(void);
	void (*flush_partial)(u32_t x, u32_t y, u32_t w, u32_t h);
	void (*cursor_update)(void);
} t_display_driver;

extern t_screen g_screen;
extern t_screen_data g_screens[MAX_SCREENS];
extern t_display_driver *display_d;

void set_term_color(u8_t color);
u8_t make_color(t_color fg, t_color bg);
u32_t color_to_rgb(t_color color);

int screen_putchar(char c);
void screen_switch(int new_id);
void screen_snap(void);
void screen_scroll(int step);
void screen_clear();

void move_cursor_to(size_t col);
void overwrite_at(size_t col, char c);

u8_t get_current_col(void);

bool line_visible(t_screen_data *s, u32_t line);
bool pinned_to_bottom(t_screen_data *s);

void screen_redraw(void);

#endif // CONSOLE_H