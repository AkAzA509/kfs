#include <arch/i386/framebuffer.h>
#include <stdio.h>
#include <kernel/multiboot.h>
#include <kernel/log.h>
#include <arch/i386/console.h>
#include <arch/i386/vga.h>
#include <kernel/init.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>

int				current_screen = 0;
t_screen		g_screen;
t_screen_data	g_screens[MAX_SCREENS];

static u32_t	g_fb_back_buffer[FB_MAX_WIDTH * FB_MAX_HEIGHT];

// #define DEBUG
#ifdef DEBUG
void	debug_current_screen(void)
{
	klog("current_screen debug:\n");
	klog("\tcol         : %lu\n", g_screens[current_screen].col);
	klog("\tcolor       : %d\n", g_screens[current_screen].color);
	klog("\thead        : %u\n", g_screens[current_screen].head);
	klog("\tview_offset : %d\n", g_screens[current_screen].view_offset);
}

void	debug_screen(void)
{
	klog("g_screen debug:\n");
	klog("\tmode     : %s\n", g_screen.mode == 1 ? "vga" : "framebuffer");
	klog("\tbuf addr : %p\n", g_screen.buf);
	klog("\tback addr: %p\n", g_screen.back_buf);
	klog("\twidth    : %d\n", g_screen.width);
	klog("\theight   : %d\n", g_screen.height);
	klog("\tpitch    : %d\n", g_screen.pitch);
	klog("\tbpp      : %d\n", g_screen.bpp);
	klog("\ttotal_row: %d\n", g_screen.total_rows);
	klog("\ttotal_col: %d\n", g_screen.total_cols);
}

void	debug_font(void)
{
	klog("font_info debug:\n");
	klog("\tbpp        : %u\n", font_info.bytesperglyph);
	klog("\tglyph_count: %u\n", font_info.glyph_count);
	klog("\theadersize : %u\n", font_info.headersize);
	klog("\theight     : %u\n", font_info.height);
	klog("\tunicode    : %s\n", font_info.unicode ? "true" : "false");
	klog("\twidth      : %d\n", font_info.width);
}
#endif // DEBUG

static void	ini_vga(void)
{
	display_d->clear();
	display_d->cursor_update();
	g_screen.total_cols = g_screen.width;
	g_screen.total_rows = g_screen.height;
}

t_font_info	font_info;
	
#define PSF1_MAGIC 0x0436
#define PSF2_MAGIC 0x864ab572

static bool	init_font(void)
{
	
	if (font_data[0] == 0x72) {
		u32_t magic = (font_data[3] << 24) | (font_data[2] << 16) |
						(font_data[1] << 8) | font_data[0];
		if (magic != PSF2_MAGIC)
			return false;

		u32_t headersize = (font_data[11] << 24) | (font_data[10] << 16) |
						(font_data[9] << 8) | font_data[8];
		u32_t flags = (font_data[15] << 24) | (font_data[14] << 16) |
						(font_data[13] << 8) | font_data[12];
		u32_t numglyphs = (font_data[19] << 24) | (font_data[18] << 16) |
						(font_data[17] << 8) | font_data[16];
		u32_t bpp = (font_data[23] << 24) | (font_data[22] << 16) |
						(font_data[21] << 8) | font_data[20];
		u32_t height = (font_data[27] << 24) | (font_data[26] << 16) |
						(font_data[25] << 8) | font_data[24];
		u32_t width = (font_data[31] << 24) | (font_data[30] << 16) |
						(font_data[29] << 8) | font_data[28];

		font_info.width = width;
		font_info.height = height;
		font_info.headersize = headersize;
		font_info.bytesperglyph = bpp;
		font_info.glyph_count = numglyphs;
		font_info.unicode = (flags & 1) ? true : false;
	}
	else if (font_data[0] == 0x36) {
		u32_t	magic = (font_data[1] << 8) | font_data[0];
		if (magic != PSF1_MAGIC)
			return false;
	
		u32_t	mode = font_data[2];
		font_info.glyph_count = (mode & 1) ? 512 : 256;
		font_info.unicode = (mode & 2) ? true : false;
	
		font_info.width = 8;
		font_info.height = font_data[3];
		font_info.headersize = 4;
		font_info.bytesperglyph = font_info.height;
	}
	else
		return false;

	g_screen.total_cols = g_screen.width / font_info.width;
	g_screen.total_rows = g_screen.height / font_info.height;

	return true;
}

static bool	init_frambuffer(void)
{
	init_font();

	if (g_screen.width > FB_MAX_WIDTH || g_screen.height > FB_MAX_HEIGHT)
		return false;

	// le curseur s'ecrit pas au demarage
	display_d->clear();

	display_d->cursor_update();
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

	set_term_color(make_color(COLOR_LIGHT_RED, COLOR_BLACK));
	printf(BOOT_LOG "terminal initialized\n");
	set_term_color(make_color(COLOR_WHITE, COLOR_BLACK));
	return true;
}

t_display_driver *display_d;

t_display_driver vga_driver = {
	.putchar_at		= putpixel_vga,
	.scroll			= scroll_physical_vga,
	.clear			= clear_physical_vga,
	.cursor_update	= update_cursor_vga,
	.flush_partial	= flush_rect_vga,
	.flush_screen	= flush_screen_vga
};

t_display_driver fb_driver = {
	.putchar_at		= render_glyph_fb,
	.scroll			= scroll_physical_fb,
	.clear			= clear_physical_fb,
	.cursor_update	= update_cursor_fb,
	.flush_partial	= flush_rect_fb,
	.flush_screen	= flush_screen_fb,
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
		display_d = &fb_driver;
	}
	else {
		init_vga();
		display_d = &vga_driver;
	}

	g_screen.cursor_col = -1;
	g_screen.cursor_row = -1;
	for (int i = 0; i < MAX_SCREENS; ++i) {
		g_screens[i].col = g_screens[i].head = 0;
		g_screens[i].color = make_color(COLOR_WHITE, COLOR_BLACK);
		memset(g_screens[i].text_buf, ' ', sizeof(g_screens[i].text_buf));
		memset(g_screens[i].color_buf, g_screens[i].color, sizeof(g_screens[i].color_buf));
	}
}