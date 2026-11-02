#include "vprint_core.h"

#include <stdio.h>

typedef struct {
	int failed;
} vprintf_ctx_t;

static void _vprint_e(void *ctx, char c)
{
	vprintf_ctx_t *vctx = ctx;
	if (putchar(c) < 0)
		vctx->failed = 1;
}

int vprintf(const char *restrict fmt, va_list ap)
{
	vprintf_ctx_t vctx = { .failed = 0 };
	out_target_t target = { .emit = _vprint_e, .ctx = &vctx, .count = 0 };

	int ret = vprint_core(fmt, &ap, &target);

	return vctx.failed ? -1 : ret;
}