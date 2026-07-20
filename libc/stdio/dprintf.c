#include <stdio.h>

int dprintf(int fd, const char *restrict fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	int ret = vdprintf(fd, fmt, ap);
	va_end(ap);
	return ret;
}