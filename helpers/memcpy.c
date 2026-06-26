#include "../includes/stddef.h"

void	*memcpy(void *dst, const void *src, size_t len)
{
	unsigned char	*d = (unsigned char *)dst;
	unsigned char	*s = (unsigned char *)src;

	while (len > 0) {
		*d = *s;
		d++;
		s++;
		len--;
	}
	return dst;
}