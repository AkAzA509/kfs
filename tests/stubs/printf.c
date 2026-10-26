#include "../../libc/stdio/vprint_core.h"
#include "../../libc/include/stdarg.h"
#include "../../libc/include/stdio.h"
#include "../include/test_common.h"

typedef struct {
	FILE *stream;
} fd_ctx_t;

static void _vfprint_e(void *ctx, char c)
{
	(void)ctx;
	real_write(&c, 1);
}

static int real_vfprintf(FILE *restrict stream, const char *restrict fmt,
			 va_list ap)
{
	fd_ctx_t fd = { .stream = stream };
	out_target_t target = { .count = 0, .ctx = &fd, .emit = _vfprint_e };

	int ret = vprint_core(fmt, &ap, &target);

	fflush(stream);
	return ret;
}

int real_printf(const char *restrict fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	int ret = real_vfprintf(stdout, fmt, ap);
	va_end(ap);
	return ret;
}
