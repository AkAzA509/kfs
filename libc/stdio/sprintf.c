#include <stdio.h>

[[gnu::format(printf, 2, 3), gnu::nonnull(1)]]
int sprintf(char *restrict str, const char *restrict fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	int ret = vsprintf(str, fmt, ap);
	va_end(ap);
	return ret;
}