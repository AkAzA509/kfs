#include <arch/i386/tty.h>
#include "vprint_core.h"
#include <stdio.h>

static void	_vprint_e(void *ctx, char c)
{
	(void)ctx;
	putchar(c);
}

int	vprintf(const char *restrict fmt, va_list ap)
{
	out_target_t target = {
		.emit = _vprint_e,
		.ctx = NULL,
		.count = 0
	};

	int ret = vprint_core(fmt, &ap, &target);

	return ret;
}