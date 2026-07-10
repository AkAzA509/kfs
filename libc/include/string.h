#ifndef HELPERS_H
#define HELPERS_H

#include <stddef.h>

size_t	strlen(const char* str);
void	*memset(void *ptr, int c, size_t len);
void	*memcpy(void *dst, const void *src, size_t len);
void	*memmove(void *dst, const void *src, size_t len);
int	memcmp(const void* s1, const void* s2, size_t len);

#endif // HELPERS_H