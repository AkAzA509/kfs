#include <core/log.h>

#include <stdio.h>
#include <stdlib.h>

int pstack(int argc, char **argv)
{
	if (argc < 0)
		fprintf(stderr, "error: pstack required an argument\n");
	if (argc == 0)
		log_stack(0);
	else
		log_stack(atoi(argv[1]));

	return 0;
}