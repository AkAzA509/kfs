#include <stdio.h>
#include "vprint_core.h"

typedef struct {
	char *buf;
	size_t size;
	size_t pos;
} snprintf_ctx_t;

static void _vsnprintf_e(void *ctx, char c)
{
	snprintf_ctx_t *sc = (snprintf_ctx_t *)ctx;
	if (sc->pos < sc->size - 1) // garder de la place pour le \0
		sc->buf[sc->pos] = c;
	sc->pos++; // continue à compter même si on n'écrit plus
}

int vsnprintf(char *restrict str, size_t size, const char *restrict fmt,
	      va_list ap)
{
	snprintf_ctx_t sc = { .buf = str, .size = size, .pos = 0 };
	out_target_t target = { .emit = _vsnprintf_e, .ctx = &sc, .count = 0 };

	vprint_core(fmt, ap, &target);

	if (size > 0)
		str[sc.pos < size ? sc.pos : size - 1] = '\0';
	return (int)sc.pos;
}