#include <stdio.h>
#include <unistd.h>
#include "vprint_core.h"

typedef struct {
	FILE *stream;
}	fd_ctx_t;

static void _vfprint_e(void *ctx, char c)
{
	fd_ctx_t *fdctx = (fd_ctx_t *)ctx;
	fputc(c, fdctx->stream);
}

int vfprintf(FILE *restrict stream, const char *restrict fmt, va_list ap)
{
	fd_ctx_t fd = { .stream = stream, };
	out_target_t target = { .count = 0, .ctx = &fd, .emit = _vfprint_e };

	int ret = vprint_core(fmt, ap, &target);

	return ret;
}