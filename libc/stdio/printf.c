#include <stdio.h>
#include <stdarg.h>

[[gnu::format(printf, 1, 2)]]
int printf(const char *restrict fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	int ret = vfprintf(stdout, fmt, ap);
	va_end(ap);
	return ret;
}