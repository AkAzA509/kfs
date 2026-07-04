#include "../helpers/kprint/kprint.h"
#include "../includes/stdint.h"
#include "../includes/config.h"
#include "multiboot.h"
#include "display.h"
#include "kernel.h"
#include "terminal.h"

#ifdef DEBUG
void	debug_diplay()
{
	kprint("g_display debug:\n");
	kprint("\tmode    : %s\n", g_display.mode == DISPLAY_FB ? "framebuffer" : "vga");
	if (g_display.mode == DISPLAY_FB)
		kprint("\tbuf addr: %p\n", g_display.buf.fb_buf);
	else
		kprint("\tbuf addr: %p\n", g_display.buf.vga_buf);
	kprint("\twidth   : %d\n", g_display.width);
	kprint("\theight  : %d\n", g_display.height);
	kprint("\tpitch   : %d\n", g_display.pitch);
	kprint("\tbpp     : %d\n", g_display.bpp);
	kprint("\tcol pos : %zu\n", g_display.col);
	kprint("\trow pos : %zu\n", g_display.row);
	kprint("\tcolor   : %d\n", g_display.color);
}
#endif

#if VIDEO_MODE == MODE_VGA
static void	init_vga(void)
{
	u16_t	line = g_display.height;
	u16_t	col = g_display.width;
	
	for (size_t y = 0; y < line * col; y++)
		putchar_vga(' ');

	g_display.row = 0;
	g_display.col = 0;
	update_cursor();
}
#endif

#if VIDEO_MODE == MODE_FRAMEBUFFER
PSF1_Header font_header;
static u32_t framebuffer_back_buffer[FB_BACK_BUFFER_PIXELS];

static void	init_frambuffer(void)
{
	font_header.charsize = font_data[3];
	font_header.mode = font_data[2];
	font_header.magic = (font_data[1] << 8) | font_data[0];

	if (font_header.charsize != 16 || font_header.magic != PSF1_FONT_MAGIC)
		return ;

	if (g_display.width > FB_MAX_WIDTH || g_display.height > FB_MAX_HEIGHT)
		return ;

	g_display.back_buf = framebuffer_back_buffer;

	u8_t *fb = (u8_t *)g_display.buf.fb_buf;
	for (u32_t y = 0; y < g_display.height; ++y) {
		u32_t *row = (u32_t *)(fb + y * g_display.pitch);
		for (u32_t x = 0; x < g_display.width; ++x)
				row[x] = 0x000000;
	}

	for (u32_t i = 0; i < g_display.width * g_display.height; ++i)
		g_display.back_buf[i] = 0x000000;

	// u16_t	line = g_display.height / font_header.charsize;
	// u16_t	col = g_display.width / 8;
	// for (size_t y = 0; y < line * col; y++)
	// 	if (y % 2 == 0)
	// 		putchar_framebuffer(' ', 0xFFFFFF, 0x0000FF);
	// 	else
	// 		putchar_framebuffer(' ', 0xFF0000, 0xFFFF00);
	
	g_display.row = 0;
	g_display.col = 0;
	update_cursor();
}
#endif

void init_term(void)
{
#if VIDEO_MODE == MODE_FRAMEBUFFER
	init_frambuffer();
#else
	init_vga();
#endif
}

void serial_print_hex(u32_t val)
{
	char hex[] = "0123456789abcdef";
	outb(0x3F8, '0');
	outb(0x3F8, 'x');
	for (int i = 7; i >=0; i--)
		outb(0x3F8, hex[(val >> (i * 4)) & 0xF]);
	outb(0x3F8, '\n');
}

void init_display(multiboot_info *mbi)
{
#if VIDEO_MODE == MODE_FRAMEBUFFER
	g_display.mode			= DISPLAY_FB;
	g_display.buf.fb_buf	= (volatile u32_t *)(u32_t)mbi->framebuffer_addr;
	g_display.width			= mbi->framebuffer_width;
	g_display.height		= mbi->framebuffer_height;
	g_display.pitch			= mbi->framebuffer_pitch;
	g_display.bpp			= mbi->framebuffer_bpp;
#else
	g_display.mode			= DISPLAY_VGA;
	g_display.buf.vga_buf	= (volatile u16_t *)0xB8000;
	g_display.width			= 80;
	g_display.height		= 25;
#endif
	g_display.col	= 0;
	g_display.row	= 0;
	g_display.color	= vga_entry_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK);
}