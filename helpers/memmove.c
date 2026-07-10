#include "../includes/stddef.h"
#include "../includes/stdbool.h"

static bool	is_overlap_cpy(size_t len, unsigned char *d, unsigned char *s)
{
	if (d > s && d < s + len) {
		d += len - 1;
		s += len - 1;
		while (len > 0) {
			*d-- = *s--;
			len--;
		}
		return true;
	}
	return false;
}

void	*memmove(void *dst, const void *src, size_t len)
{
	unsigned char	*d = (unsigned char *)dst;
	unsigned char	*s = (unsigned char *)src;

	if (len == 0 || src == NULL || dst == NULL)
		return (dst);

	if (!is_overlap_cpy(len, d, s)) {
		while (len > 0) {
			*d++ = *s++;
			len--;
		}
	}

	return dst;
}