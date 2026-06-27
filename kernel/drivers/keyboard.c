#include "keyboard.h"
#include "../../includes/kernel.h"
#include "../../includes/stdbool.h"

static bool shift = false;
static bool caps_lock = false;

// read 1 byte from the port
static inline u8_t inb(u16_t port)
{
	u8_t val;

	// inb = read from a hardware port
	// place the output 'a' into val, read from 'Nd'=port
	__asm__ volatile ("inb %1, %0" : "=a"(val) : "Nd"(port));
	return val;
}

// duplicate function into ../kwrite.c need to factorize the code
// write 1 byte in the port
static inline void outb(u16_t port, u8_t val)
{
	// outb = write into a hardware port
	// place the input 'a' into val, send to 'Nd'=port
	__asm__ volatile ("outb %0, %1" : : "a"(val), "Nd"(port));
}

static void	handle_scancode(u8_t code)
{
	if (code & RELEASE_MSK) {
		u8_t key = code & ~RELEASE_MSK;

		if (key == LEFT_SHIFT || key == RIGHT_SHIFT)
			shift = false;
		return ;
	}
	if (code == LEFT_SHIFT || code == RIGHT_SHIFT) {
		shift = true;
		return ;
	}
	if (code == CAPS_LOCK)
		caps_lock = !caps_lock;

	char val = keycode[code];
	if (!val)
		return ;

	if (shift && !caps_lock)
		val = keycode_shift[code];
	else if (caps_lock && !shift && val >= 'a' && val <= 'z')
		val = keycode_shift[code];

	kputchar(val);
}

void	keyboard_handler()
{
	while (1)
	{
		// 0x64 register the status of the port (ready or not, busy ...)
		if (inb(0x64) & 0x01)
		{
			u8_t scancode = inb(0x60);
			handle_scancode(scancode);
		}
	}
}