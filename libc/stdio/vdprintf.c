#include <stdio.h>
#include <unistd.h>
#include "vprint_core.h"

static void _vdprint_e(void *ctx, char c)
{
	write((int)ctx, &c, 1);
}

int vdprintf(int fd, const char *restrict fmt, va_list ap)
{
	out_target_t target = { .count = 0, .ctx = &fd, .emit = _vdprint_e };

	int ret = vprint_core(fmt, &ap, &target);

	return ret;
}