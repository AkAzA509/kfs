#include <stddef.h>

void *memcpy(void *restrict dst, const void *restrict src, size_t len)
{
	unsigned char *d = (unsigned char *)dst;
	const unsigned char *s = (const unsigned char *)src;

	for (size_t i = 0; i < len; ++i)
		d[i] = s[i];

	return dst;
}