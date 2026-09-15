#include <stdio.h>

int dprintf(int fd, const char *restrict fmt, ...)
{
	va_list ap;
	FILE tmp = {
		.fd = fd,
		.mode = _IOLBF,
	};

	va_start(ap, fmt);
	int ret = vfprintf(&tmp, fmt, ap);
	va_end(ap);
	return ret;
}