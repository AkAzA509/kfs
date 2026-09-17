#include <stddef.h>
#include <stdint.h>

[[gnu::nonnull(1, 2)]]
void *memmove(void *dst, const void *src, size_t len)
{
	unsigned char *d = (unsigned char *)dst;
	const unsigned char *s = (const unsigned char *)src;

	if (len == 0)
		return dst;

	if ((uintptr_t)dst < (uintptr_t)src) {
		for (size_t i = 0; i < len; ++i)
			d[i] = s[i];
	} else {
		for (size_t i = len; i != 0; --i)
			d[i - 1] = s[i - 1];
	}

	return dst;
}