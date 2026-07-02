#include "../includes/config.h"
#include "../kernel/terminal.h"
#include "../kernel/display.h"

void	kputchar(char c)
{
	#if VIDEO_MODE == MODE_FRAMEBUFFER
		putchar_framebuffer(c, 0, 0);
	#else
		putchar_vga(c);
		update_cursor();
	#endif
}