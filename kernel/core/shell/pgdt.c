#include <arch/i386/gdt.h>

int pgdt(int argc, char **argv)
{
	(void)argc;
	(void)argv;

	print_gdt();

	return 0;
}