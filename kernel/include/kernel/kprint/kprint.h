#ifndef KPRINT_H
#define KPRINT_H

#include <stddef.h>

int	kprint(const char *str, ...);
void	kwrite(const void* data, size_t size);
void	kputchar(char c);

#endif // KPRINT_H
