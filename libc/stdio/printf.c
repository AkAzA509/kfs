#include <stdio.h>
#include <stdarg.h>

int	printf(const char *restrict fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	int ret = vprintf(fmt, ap);
	va_end(ap);
	return ret;
}