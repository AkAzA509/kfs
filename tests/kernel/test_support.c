#include "test_support.h"

#define SERIAL_PORT 0x3f8
#define DEBUG_EXIT_PORT 0x501

static void outb(u16_t port, u8_t value)
{
	__asm__ volatile("outb %0, %1" : : "a"(value), "Nd"(port));
}

static u8_t inb(u16_t port)
{
	u8_t value;

	__asm__ volatile("inb %1, %0" : "=a"(value) : "Nd"(port));
	return value;
}

/* Configure COM1 for kernel test output. */
void test_serial_init(void)
{
	outb(SERIAL_PORT + 1, 0x00);
	outb(SERIAL_PORT + 3, 0x80);
	outb(SERIAL_PORT + 0, 0x03);
	outb(SERIAL_PORT + 1, 0x00);
	outb(SERIAL_PORT + 3, 0x03);
	outb(SERIAL_PORT + 2, 0xc7);
	outb(SERIAL_PORT + 4, 0x0b);
}

static void serial_putc(char value)
{
	while ((inb(SERIAL_PORT + 5) & 0x20) == 0)
		;
	outb(SERIAL_PORT, (u8_t)value);
}

/* Send a null-terminated string through COM1. */
void test_serial_puts(const char *value)
{
	while (*value != '\0')
		serial_putc(*value++);
}

/* Tell QEMU that the test is complete, then stop the CPU. */
__attribute__((noreturn)) void test_exit(u8_t code)
{
	outb(DEBUG_EXIT_PORT, code);
	for (;;)
		__asm__ volatile("cli; hlt");
}
