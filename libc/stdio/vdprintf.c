#include <stdio.h>
#include <unistd.h>
#include "vprint_core.h"

typedef struct {
	int fd;
} fd_ctx_t;

static void _vdprint_e(void *ctx, char c)
{
	fd_ctx_t *fdctx = (fd_ctx_t *)ctx;
	write(fdctx->fd, &c, 1);
}

int vdprintf(int fd, const char *restrict fmt, va_list ap)
{
	fd_ctx_t fd_ctx = {
		.fd = fd,
	};
	out_target_t target = { .count = 0,
				.ctx = &fd_ctx,
				.emit = _vdprint_e };

	int ret = vprint_core(fmt, &ap, &target);

	return ret;
}
