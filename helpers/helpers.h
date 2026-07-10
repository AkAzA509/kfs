#ifndef HELPERS_H
#define HELPERS_H

#include "../includes/stddef.h"

size_t	strlen(const char* str);
void	*memset(void *ptr, int c, size_t len);
void	*memcpy(void *dst, const void *src, size_t len);
void	*memmove(void *dst, const void *src, size_t len);
void	kwrite(const void* data, size_t size);
void	kputchar(char c);

#endif // HELPERS_H