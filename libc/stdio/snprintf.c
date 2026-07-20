#include <stdio.h>

// The functions snprintf() do not write more than size bytes
// (including the terminating null byte ('\0')). If the output was truncated
// due to this limit then the return value is the number of characters
// (excluding the terminating null byte) which would have been written to
// the final string if enough space had been available.
// Thus, a return value of size or more means that the output was truncated.
int snprintf(char *restrict str, size_t size, const char *restrict fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	int ret = vsnprintf(str, size, fmt, ap);
	va_end(ap);
	return ret;
}