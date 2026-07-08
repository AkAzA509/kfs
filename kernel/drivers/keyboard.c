#include "../../includes/stdbool.h"
#include "../kernel.h"
#include "../terminal.h"
#include "keyboard.h"

static const u8_t RELEASE_MSK = 0x80;
static const u8_t LEFT_SHIFT = 0x2A;
static const u8_t RIGHT_SHIFT = 0x36;
static const u8_t CAPS_LOCK = 0x3A;

static const char keycode[] = {
	0, 0, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b',	// 0x00-0x0E (0x01 = escape)
	0, 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',	// 0x0F-0x1C
	0, 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`',			// 0x1D-0x29
	0, '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', 0,			// 0x2A-0x36
	'*', 0, ' ', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,							// 0x37-0x44 (* numpad, left alt, space, capslock, f1 to f10)
	0, 0, '7', '8', '9', '-', '4', '5', '6', '+', '1', '2', '3', '0', '.',	// 0x45-53 (number lock, scrolllock)
	0, 0, 0, 0, 0															// 0x54 to 0x56 empty 0x57-0x58 (f11, f12)
};

static const char keycode_shift[] = {
	0, 0, '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+', '\b',	// 0x00-0x0E
	0, 'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', '\n',	// 0x0F-0x1C
	0, 'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', '"', '~',			// 0x1D-0x29
	0, '|', 'Z', 'X', 'C', 'V', 'B', 'N', 'M', '<', '>', '?', 0,			// 0x2A-0x36
	'*', 0, ' ', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,							// 0x37-0x44 (* numpad, left alt, space, capslock, f1 to f10)
	0, 0, '7', '8', '9', '-', '4', '5', '6', '+', '1', '2', '3', '0', '.',	// 0x45-53 (number lock, scrolllock)
	0, 0, 0, 0, 0															// 0x54 to 0x56 empty 0x57-0x58 (f11, f12)
};

static bool shift = false;
static bool caps_lock = false;

static void	handle_screen_switch(u8_t code)
{
	switch (code) {
		case 0x3b:
			screen_switch(0);	// f1
			return ;

		case 0x3c:
			screen_switch(1);	// f2
			return ;

		case 0x3d:
			screen_switch(2);	// f3
			return ;

		case 0x3e:
			screen_switch(3);	// f4
			return ;

		default:
			break ;
	}
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


	if (code >= 0x3b && code <= 0x3e)	// f1 to f4
		handle_screen_switch(code);
	if (code == 0x0e) {
		backspace();
		return ;
	}

	char val = keycode[code];
	if (!val)
		return ;

	if (shift && !caps_lock)
		val = keycode_shift[code];
	else if ((caps_lock && shift && (val < 'a' || val > 'z')) ||
			(caps_lock && !shift && val >= 'a' && val <= 'z'))
		val = keycode_shift[code];

	putchar(val);
}

void	keyboard_handler()
{
	while (1)
	{
		// 0x64 register give the port's status (ready or not, busy ...)
		if (inb(0x64) & 0x01)
		{
			u8_t scancode = inb(0x60); // 0x60 register give the data (key press)
			handle_scancode(scancode);
		}
	}
}