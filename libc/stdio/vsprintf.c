#include <stdio.h>
#include "vprint_core.h"

typedef struct {
	char *buf;
	size_t pos;
} sprintf_ctx_t;

static void _vsprintf_e(void *ctx, char c)
{
	sprintf_ctx_t *sc = (sprintf_ctx_t *)ctx;

	sc->buf[sc->pos] = c;
	sc->pos++;
}

int vsprintf(char *restrict str, const char *restrict fmt, va_list ap)
{
	sprintf_ctx_t sc = { .buf = str, .pos = 0 };
	out_target_t target = { .emit = _vsprintf_e, .ctx = &sc, .count = 0 };

	vprint_core(fmt, ap, &target);

	str[sc.pos] = '\0';
	return (int)sc.pos;
}