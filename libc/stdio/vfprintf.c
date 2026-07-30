#include <stdio.h>
#include <unistd.h>
#include "vprint_core.h"

static void _vfprint_e(void *ctx, char c)
{
	write((int)ctx, &c, 1);
}

int vfprintf(FILE *restrict stream, const char *restrict fmt, va_list ap)
{
	out_target_t target = { .count = 0, .ctx = stream, .emit = _vfprint_e };

	int ret = vprint_core(fmt, &ap, &target);

	return ret;
}