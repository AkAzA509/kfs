#include "../includes/stdint.h"
#include "kernel.h"

// ===== In io function ===== //

// "=a"(ret): the '=' means that it is an output (the result of the instruction)
// stored in register a (AL/AX/EAX depending on the size of ret)

// read a long (32bits, 4 bytes) from the io port
inline u32_t	inl(u16_t port)
{
	u32_t ret;
	__asm__ volatile ("inl %1, %0" : "=a"(ret) : "Nd"(port));
	return ret;
}

// read a word (16bits, 2 bytes) from the io port
inline u16_t	inw(u16_t port)
{
	u16_t ret;
	__asm__ volatile ("inw %1, %0" : "=a"(ret) : "Nd"(port));
	return ret;
}

// read a byte (8bits) from the io port
inline u8_t	inb(u16_t port)
{
	u8_t ret;
	__asm__ volatile ("inb %1, %0" : "=a"(ret) : "Nd"(port));
	return ret;
}

// ===== Out io function ===== //

// write a long (32bits, 4 bytes) into the io port
inline void	outl(u16_t port, u32_t val)
{
	__asm__ volatile ("outl %0, %1" : : "a"(val), "Nd"(port));
}

// write a word (16bits, 2 bytes) into the io port
inline void	outw(u16_t port, u16_t val)
{
	__asm__ volatile ("outw %0, %1" : : "a"(val), "Nd"(port));
}

// write a byte (8bits) into the io port
// place the input val into 'a', send to 'Nd'=port
inline void	outb(u16_t port, u8_t val)
{
	__asm__ volatile ("outb %0, %1" : : "a"(val), "Nd"(port));
}