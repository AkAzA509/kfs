#include <stddef.h>

void	*memset(void *ptr, int c, size_t len)
{
	unsigned char	*p = (unsigned char *)ptr;

	for (size_t i = 0; i < len; ++i)
		p[i] = (unsigned char)c;

	return ptr;
}