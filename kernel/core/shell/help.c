#include <stdio.h>

int help(int argc, char **argv)
{
	(void)argc;
	(void)argv;

	printf("here is a list of the command available on this shell\n\n");
	printf("clear              clear the current screen\n");
	printf("date               display the current date in UTC+2 format\n");
	printf("echo <string>      echo the next arguments\n");
	printf("halt               halt the cpu and lock it (for now)\n");
	printf("help               display this informations\n");
	printf("layout <name>      change the keyboard layout\n");
	printf("plogo              print the kernel boot logo\n");
	printf("pgdt               print the gdt base address\n");
	printf("pscreen            print the g_screen info\n");
	printf("pstack <val>       print the kernel stack, by default all the stack is displayed\n");
	printf("                   add a positive val to chose the number of element displayed\n");
	printf("reboot             reboot the os\n");
	printf("shutdown           shutdown the os\n");

	return 0;
}