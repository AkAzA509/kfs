#ifndef KERNEL_H
#define KERNEL_H

#include "stddef.h"
#include "stdint.h"
#include "../helpers/kprint/kprint.h"

void	keyborad_handler();

void	kwrite(const void* data, size_t size);
void	kputchar(char c);
void	set_term_color(u8_t color);

int		kprint(const char *format, ...);
size_t	strlen(const char* str);
void	*memset(void *ptr, int c, size_t len);
void	*memcpy(void *dst, const void *src, size_t len);

#endif // KERNEL_H