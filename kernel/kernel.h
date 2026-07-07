#ifndef KERNEL_H
#define KERNEL_H

#include "../includes/stdint.h"
#include "../includes/stddef.h"

void	outb(u16_t port, u8_t val);
u8_t	inb(u16_t port);
void	serial_print_hex(u32_t val);

#endif // KERNEL_H