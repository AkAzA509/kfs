#ifndef STRING_H
#define STRING_H

#include <stddef.h>

size_t	strlen(const char* str);
void	*memset(void *ptr, int c, size_t len);
void	*memcpy(void *dst, const void *src, size_t len);
void	*memmove(void *dst, const void *src, size_t len);
int		memcmp(const void* s1, const void* s2, size_t len);

#endif // STRING_H