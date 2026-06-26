#include "../includes/stddef.h"

void	*memset(void *ptr, int c, size_t len)
{
	unsigned char	*p = (unsigned char *)ptr;

	while (len > 0) {
		*p = (unsigned char)c;
		len--;
		p++;
	}
	return ptr;
}