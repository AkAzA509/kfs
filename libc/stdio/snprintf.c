#include <stdio.h>

int snprintf(char *restrict str, size_t size, const char *restrict fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	int ret = vsnprintf(str, size, fmt, ap);
	va_end(ap);
	return ret;
}