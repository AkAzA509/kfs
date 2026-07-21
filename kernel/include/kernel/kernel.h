#ifndef KERNEL_H
#define KERNEL_H

#include <stdint.h>

void	outb(u16_t port, u8_t val);
void	outw(u16_t port, u16_t val);
void	outl(u16_t port, u32_t val);
u8_t	inb(u16_t port);
u16_t	inw(u16_t port);
u32_t	inl(u16_t port);
void	serial_print_hex(u32_t val);

void	init_gdt();

int	test_printf_run(void);

#endif // KERNEL_H