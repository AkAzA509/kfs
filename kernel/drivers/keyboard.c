#include "keyboard.h"
#include "../kernel_internal.h"
#include "../../includes/kernel.h"
#include "../../includes/stdbool.h"

static bool shift = false;
static bool caps_lock = false;

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
	else if ((caps_lock && shift && (val < 'a' || val > 'z')) ||
			(caps_lock && !shift && val >= 'a' && val <= 'z'))
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