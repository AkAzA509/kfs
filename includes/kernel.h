#ifndef KERNEL_H
#define KERNEL_H

#include "stddef.h"
#include "stdint.h"

void	keyboard_handler();

void	kwrite(const void* data, size_t size);
void	kputchar(char c);

#endif // KERNEL_H