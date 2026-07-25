#include "stdint.h"
#include <arch/i386/keyboard.h>
#include <kernel/kernel.h>
#include <kernel/log.h>
#include <arch/i386/tty.h>
#include <stdbool.h>
#include <stdio.h>

#define RELEASE_MSK 0x80
#define LEFT_SHIFT 0x2A
#define RIGHT_SHIFT 0x36
#define CAPS_LOCK 0x3A
#define HOME 0x47

#define ARROW_UP 0x48
#define PAGE_UP 0x49
#define ARROW_DOWN 0x50
#define PAGE_DOWN 0x51

static const char keycode[] = {
	0, 0, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b',	// 0x00-0x0E (0x01 = escape)
	'\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',	// 0x0F-0x1C
	0, 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`',			// 0x1D-0x29
	0, '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', 0,			// 0x2A-0x36
	'*', 0, ' ', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,							// 0x37-0x44 (* numpad, left alt, space, capslock, f1 to f10)
	0, 0, '7', '8', '9', '-', '4', '5', '6', '+', '1', '2', '3', '0', '.',	// 0x45-53 (number lock, scrolllock)
	0, 0, 0, 0, 0															// 0x54 to 0x56 empty 0x57-0x58 (f11, f12)
};

static const char keycode_shift[] = {
	0, 0, '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+', '\b',	// 0x00-0x0E
	'\t', 'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', '\n',	// 0x0F-0x1C
	0, 'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', '"', '~',			// 0x1D-0x29
	0, '|', 'Z', 'X', 'C', 'V', 'B', 'N', 'M', '<', '>', '?', 0,			// 0x2A-0x36
	'*', 0, ' ', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,							// 0x37-0x44 (* numpad, left alt, space, capslock, f1 to f10)
	0, 0, '7', '8', '9', '-', '4', '5', '6', '+', '1', '2', '3', '0', '.',	// 0x45-53 (number lock, scrolllock)
	0, 0, 0, 0, 0															// 0x54 to 0x56 empty 0x57-0x58 (f11, f12)
};

static bool shift = false;
static bool caps_lock = false;
static bool extended_pending = false;

static void	handle_screen_switch(u8_t code)
{
	switch (code) {
		case 0x3b: screen_switch(0); return ; // f1
		case 0x3c: screen_switch(1); return ; // f2
		case 0x3d: screen_switch(2); return ; // f3
		case 0x3e: screen_switch(3); return ; // f4
		default: break ;
	}
}

// Fake exit, to modify when the kernel has memory
void	handle_exit(void)
{
	printf("Shuting down ...\n");
	outw(0x604, 0x2000);
}

static void	handle_scroll_arrow(u8_t code, bool is_release)
{
	if (is_release)
		return ;

	switch (code) {
		case ARROW_UP: screen_scroll(-1); break ;
		case ARROW_DOWN: screen_scroll(1); break ;
		case PAGE_UP: screen_scroll(-(int)g_screen.total_rows); break ;
		case PAGE_DOWN: screen_scroll((int)g_screen.total_rows); break ;
		case HOME: screen_snap(); break ;
		default: break ;
	}
}

// For the key with 2 code
static void	handle_extended_key(u8_t code, bool is_release)
{
	if (code >= 0x47 && code <= 0x51) // HOME to PAGE DOWN
		handle_scroll_arrow(code, is_release);
}

static void	handle_release_special_key(u8_t code)
{
	u8_t key = code & ~RELEASE_MSK;

	if (key == LEFT_SHIFT || key == RIGHT_SHIFT)
		shift = false;
	return ;
}

static void	handle_press_special_key(u8_t code)
{
	if (code == LEFT_SHIFT || code == RIGHT_SHIFT) {
		shift = true;
		return ;
	}
	if (code == CAPS_LOCK)
		caps_lock = !caps_lock;
}

static bool	handle_single_key(u8_t code, bool is_release)
{
	if (is_release) {
		handle_release_special_key(code);
		return false;
	}
	if (code == LEFT_SHIFT || code == RIGHT_SHIFT || code == CAPS_LOCK) {
		handle_press_special_key(code);
		return false;
	}
	if (code == 0x01) {
		handle_exit();
		return false;
	}
	if (code >= 0x3b && code <= 0x3e) {
		handle_screen_switch(code);
		return false;
	}
	if (code == 0x0e) {
		backspace();
		return false;
	}
	return true;
}

static void	print_code(u8_t code)
{
	char val = keycode[code];
	if (!val)
		return ;

	if (shift && !caps_lock)
		val = keycode_shift[code];
	else if ((caps_lock && shift && (val < 'a' || val > 'z')) ||
			(caps_lock && !shift && val >= 'a' && val <= 'z'))
		val = keycode_shift[code];

	if (!pinned_to_bottom(&g_screens[current_screen]))
		screen_snap();
	kputchar(val);
}

static void	read_scancode(u8_t scancode)
{
	if (scancode == 0xE0) {
		extended_pending = true;
		return ;
	}

	bool	is_extended = extended_pending;
	bool	is_release = scancode & 0x80;
	u8_t	code = scancode & 0x7F;

	extended_pending = false;

	if (is_extended) {
		handle_extended_key(code, is_release);
		return ;
	}
	if (!handle_single_key(code, is_release))
		return ;

	#ifdef DEBUG
		serial_print_hex(code);
	#endif

	print_code(code);
}

void	keyboard_handler()
{
	while (1)
	{
		// 0x64 register give the port's status (ready or not, busy ...)
		if (inb(0x64) & 0x01)
		{
			u8_t scancode = inb(0x60); // 0x60 register give the data (key press)
			read_scancode(scancode);
		}
	}
}