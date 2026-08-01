#ifndef VPRINT_CORE_H
#define VPRINT_CORE_H

#include <stdarg.h>
#include <stddef.h>

typedef struct {
	void (*emit)(void *ctx, char c);
	void *ctx;
	size_t count; // total of char product for retrun value
} out_target_t;

int vprint_core(const char *restrict fmt, va_list ap, out_target_t *target);

#endif // VPRINT_CORE_H