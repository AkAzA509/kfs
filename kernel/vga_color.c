#include "../includes/stdint.h"
#include "display.h"

void set_term_color(u8_t color)
{
	g_display.color = color;
}