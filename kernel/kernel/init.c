#include <arch/i386/framebuffer.h>
#include <stdio.h>
#include <kernel/multiboot.h>
#include <arch/i386/tty.h>
#include <arch/i386/vga.h>
#include <kernel/init.h>
#include <stdbool.h>
#include <stdint.h>

int				current_screen = 0;
t_screen		g_screen;
t_screen_data	g_screens[MAX_SCREENS];

static u32_t	g_fb_back_buffer[FB_MAX_WIDTH * FB_MAX_HEIGHT];

#ifdef DEBUG
void	debug_diplay()
{
	printf("g_screen debug:\n");
	printf("\tmode     : %s\n", g_screen.mode == 1 ? "vga" : "framebuffer");
	printf("\tbuf addr : %p\n", g_screen.buf);
	printf("\tback addr: %p\n", g_screen.back_buf);
	printf("\twidth    : %d\n", g_screen.width);
	printf("\theight   : %d\n", g_screen.height);
	printf("\tpitch    : %d\n", g_screen.pitch);
	printf("\tbpp      : %d\n", g_screen.bpp);
	printf("\tcol pos  : %zu\n", g_screen.col);
	printf("\trow pos  : %zu\n", g_screen.row);
	printf("\tcolor    : %d\n", g_screen.color);
}
#endif // DEBUG

static void	ini_vga(void)
{
	current_driver->clear();
	update_cursor();
}

PSF1_Header	font_header;

static bool	init_frambuffer(void)
{
	font_header.charsize = font_data[3];
	font_header.mode = font_data[2];
	font_header.magic = (font_data[1] << 8) | font_data[0];

	if (font_header.charsize != 16 || font_header.magic != PSF1_FONT_MAGIC)
		return false;
	if (g_screen.width > FB_MAX_WIDTH || g_screen.height > FB_MAX_HEIGHT)
		return false;

	// le curseur s'acrit pas au demarage
	current_driver->clear();

	update_cursor();
	return true;
}

bool init_term(void)
{
	if (g_screen.mode == 0) {
		if (!init_frambuffer())
			return false;
	}
	else
		ini_vga();
	return true;
}

t_display_driver *current_driver;

t_display_driver vga_driver = {
	.putchar	= putchar_vga,
	.scroll		= scroll_vga,
	.clear		= clear_vga,
};

t_display_driver fb_driver = {
	.putchar	= putchar_fb,
	.scroll		= scroll_fb,
	.clear		= clear_fb,
};

static void	init_fb(multiboot_info *mbi)
{
	g_screen.mode		= 0;
	g_screen.buf		= (void *)(u32_t)mbi->framebuffer_addr;
	g_screen.back_buf	= g_fb_back_buffer;
	g_screen.width		= mbi->framebuffer_width;
	g_screen.height		= mbi->framebuffer_height;
	g_screen.pitch		= mbi->framebuffer_pitch;
	g_screen.bpp		= mbi->framebuffer_bpp;
}

static void	init_vga(void)
{
	g_screen.mode		= 1;
	g_screen.buf		= (void *)0xB8000;
	g_screen.back_buf	= NULL;
	g_screen.width		= 80;
	g_screen.height		= 25;
	g_screen.bpp		= 16;
	g_screen.pitch		= 80 * 2;
}

void init_display(multiboot_info *mbi)
{
	if (mbi->flags & MULTIBOOT_INFO_FRAMEBUFFER_INFO && mbi->framebuffer_type == 1) {
		init_fb(mbi);
		current_driver = &fb_driver;
	}
	else {
		init_vga();
		current_driver = &vga_driver;
	}
	g_screen.col = g_screen.row = 0;
	g_screen.color = make_color(COLOR_WHITE, COLOR_BLACK);

	for (int i = 0; i < MAX_SCREENS; ++i) {
		g_screens[i].col = g_screens[i].row = 0;
		g_screens[i].color = g_screen.color;
	}
}