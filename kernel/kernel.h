#ifndef KERNEL_H
#define KERNEL_H

#include "../includes/stdint.h"
#include "../includes/stddef.h"

#define ATTRIBUTE	7

// I/O

void outb(u16_t port, u8_t val);
u8_t inb(u16_t port);

#endif // KERNEL_H