#include <string.h>
#include <stddef.h>

[[gnu::nonnull(1)]] void *memset(void *ptr, int c, size_t len)
{
	unsigned char *p = (unsigned char *)ptr;

	for (size_t i = 0; i < len; ++i)
		p[i] = (unsigned char)c;

	return ptr;
}