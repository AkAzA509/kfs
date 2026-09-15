#include <stdio.h>

int fprintf(FILE *restrict stream, const char *restrict fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	int ret = vfprintf(stream, fmt, ap);
	va_end(ap);
	return ret;
}