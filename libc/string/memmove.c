#include <stddef.h>

void *memmove(void *dst, const void *src, size_t len)
{
	unsigned char *d = (unsigned char *)dst;
	const unsigned char *s = (const unsigned char *)src;

	if (len == 0 || src == NULL || dst == NULL)
		return (dst);

	if (d < s) {
		for (size_t i = 0; i < len; ++i)
			d[i] = s[i];
	} else {
		for (size_t i = len; i != 0; --i)
			d[i - 1] = s[i - 1];
	}

	return dst;
}