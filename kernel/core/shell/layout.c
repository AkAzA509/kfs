#include <drivers/keyboard.h>
#include <stdio.h>

int layout(int argc, char **argv)
{
	if (argc <= 1)
		printf("error: layout required a layout name\n");

	change_layout(argv[1]);
	return 0;
}