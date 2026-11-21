#include <core/common.h>

int halt(int argc, char **argv)
{
	(void)argc;
	(void)argv;

	__asm__ volatile("cli");
	HALT_ERROR;
}