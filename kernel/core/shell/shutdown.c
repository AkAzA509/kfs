#include <core/common.h>

int shutdown(int argc, char **argv)
{
	(void)argc;
	(void)argv;

	SHUTDOWN;
	HALT_ERROR;
}