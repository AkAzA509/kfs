#include "../helpers/kprint/kprint.h"
#include "./drivers/framebuffer.h"
#include "../includes/stdint.h"
#include "./drivers/vga.h"
#include "multiboot.h"
#include "terminal.h"
#include "init.h"

int				current_screen = 0;
t_screen		g_screen;
t_screen_data	g_screens[MAX_SCREENS];

static u32_t	g_fb_back_buffer[FB_MAX_WIDTH * FB_MAX_HEIGHT];

#ifdef DEBUG
void	debug_diplay()
{
	kprint("g_screen debug:\n");
	kprint("\tmode    : %s\n", g_screen.mode == 1 ? "vga" : "framebuffer");
	kprint("\tbuf addr: %p\n", g_screen.buf);
	kprint("\twidth   : %d\n", g_screen.width);
	kprint("\theight  : %d\n", g_screen.height);
	kprint("\tpitch   : %d\n", g_screen.pitch);
	kprint("\tbpp     : %d\n", g_screen.bpp);
	kprint("\tcol pos : %zu\n", g_screen.col);
	kprint("\trow pos : %zu\n", g_screen.row);
	kprint("\tcolor   : %d\n", g_screen.color);
}
#endif // DEBUG

static void	ini_vga(void)
{
	const size_t total_size = g_screen.width * g_screen.height;
	u16_t *vga_mem = (u16_t *)g_screen.buf;

	for (size_t i = 0; i < total_size; ++i)
		vga_mem[i] = vga_entry(' ', g_screen.color);

	g_screen.row = 0;
	g_screen.col = 0;
	update_cursor();
}

PSF1_Header	font_header;

static void	init_frambuffer(void)
{
	font_header.charsize = font_data[3];
	font_header.mode = font_data[2];
	font_header.magic = (font_data[1] << 8) | font_data[0];

	if (font_header.charsize != 16 || font_header.magic != PSF1_FONT_MAGIC)
		return ;
	if (g_screen.width > FB_MAX_WIDTH || g_screen.height > FB_MAX_HEIGHT)
		return ;

	current_driver->clear();

	// back buffer cleaning
	for (u32_t i = 0; i < g_screen.width * g_screen.height; ++i)
		g_screen.back_buf[i] = 0x000000;

	g_screen.row = 0;
	g_screen.col = 0;
	update_cursor();
}

void init_term(void)
{
	if (g_screen.mode == 0)
		init_frambuffer();
	else
		ini_vga();
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
	} else {
		init_vga();
		current_driver = &vga_driver;
	}
	g_screen.col	= 0;
	g_screen.row	= 0;
	g_screen.color	= g_screen.color = make_color(COLOR_WHITE, COLOR_BLACK);

	for (int i = 0; i < MAX_SCREENS; ++i) {
		g_screens[i].col = 0;
		g_screens[i].row = 0;
		g_screens[i].color = g_screen.color;
	}
}