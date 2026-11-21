#include <core/common.h>

int reboot(int argc, char **argv)
{
	(void)argc;
	(void)argv;

	REBOOT;
	HALT_ERROR;
}