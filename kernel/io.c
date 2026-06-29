#include "../../includes/stdint.h"

// read 1 byte from port
static inline u8_t inb(u16_t port)
{
	u8_t val;

	// inb = read from a hardware port
	// place the output 'a' into val, read from 'Nd'=port
	__asm__ volatile ("inb %1, %0" : "=a"(val) : "Nd"(port));
	return val;
}

// write 1 byte in port
static inline void outb(u16_t port, u8_t val)
{
	// outb = write into a hardware port
	// place the input 'a' into val, send to 'Nd'=port
	__asm__ volatile ("outb %0, %1" : : "a"(val), "Nd"(port));
}