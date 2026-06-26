#ifndef KEYBOARD_H
#define KEYBOARD_H

#include "../../includes/stdint.h"

static u8_t RELEASE_MSK = 0x80;
static u8_t LEFT_SHIFT = 0x2A;
static u8_t RIGHT_SHIFT = 0x36;
static u8_t CAPS_LOCK = 0x3A;

static const char keycode[] = {
	0, 0, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', 0,	// 0x00-0x0E (0x01 = escape)
	0, 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', 0,		// 0x0F-0x1C
	0, 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`',			// 0x1D-0x29
	0, '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', 0,			// 0x2A-0x36
	'*', 0, ' ', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,							// 0x37-0x44 (* numpad, left alt, space, capslock, f1 to f10)
	0, 0, '7', '8', '9', '-', '4', '5', '6', '+', '1', '2', '3', '0', '.',	// 0x45-53 (number lock, scrolllock)
	0, 0, 0, 0, 0															// 0x54 to 0x57 empty 0x58-0x58 (f11, f12)
};

static const char keycode_shift[] = {
	0, 0, '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+', 0,	// 0x00-0x0E
	0, 'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', 0,		// 0x0F-0x1C
	0, 'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', '"', '~',			// 0x1D-0x29
	0, '|', 'Z', 'X', 'C', 'V', 'B', 'N', 'M', '<', '>', '?', 0,			// 0x2A-0x36
	'*', 0, ' ', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,							// 0x37-0x44 (* numpad, left alt, space, capslock, f1 to f10)
	0, 0, '7', '8', '9', '-', '4', '5', '6', '+', '1', '2', '3', '0', '.',	// 0x45-53 (number lock, scrolllock)
	0, 0, 0, 0, 0															// 0x54 to 0x57 empty 0x58-0x58 (f11, f12)
};

#endif // KEYBOARD_H