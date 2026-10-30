#include <stdio.h>

int sprintf(char *restrict str, const char *restrict fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	int ret = vsprintf(str, fmt, ap);
	va_end(ap);
	return ret;
}