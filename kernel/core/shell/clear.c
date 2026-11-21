#include <drivers/console.h>

int clear(int argc, char **argv)
{
	(void)argc;
	(void)argv;

	screen_clear(&g_screens[current_screen]);

	return 0;
}