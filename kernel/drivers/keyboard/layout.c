#include <stddef.h>
#include <string.h>

// clang-format off
static const char layout_qwerty_lower[] = {
	0, 0, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b',	// 0x00-0x0E (0x01 = escape)
	'\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',	// 0x0F-0x1C
	0, 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`',			// 0x1D-0x29
	0, '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', 0,			// 0x2A-0x36
	'*', 0, ' ', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,							// 0x37-0x44 (* numpad, left alt, space, capslock, f1 to f10)
	0, 0, '7', '8', '9', '-', '4', '5', '6', '+', '1', '2', '3', '0', '.',	// 0x45-53 (number lock, scrolllock)
	0, 0, 0, 0, 0															// 0x54 to 0x56 empty 0x57-0x58 (f11, f12)
};

static const char layout_qwerty_upper[] = {
	0, 0, '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+', '\b',	// 0x00-0x0E
	'\t', 'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', '\n',	// 0x0F-0x1C
	0, 'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', '"', '~',			// 0x1D-0x29
	0, '|', 'Z', 'X', 'C', 'V', 'B', 'N', 'M', '<', '>', '?', 0,			// 0x2A-0x36
	'*', 0, ' ', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,							// 0x37-0x44 (* numpad, left alt, space, capslock, f1 to f10)
	0, 0, '7', '8', '9', '-', '4', '5', '6', '+', '1', '2', '3', '0', '.',	// 0x45-53 (number lock, scrolllock)
	0, 0, 0, 0, 0															// 0x54 to 0x56 empty 0x57-0x58 (f11, f12)
};

static const char layout_azerty_lower[] = {
	0, 0, '&', 0x82, '"', '\'', '(', '-', 0x8A, '_', 0x87, 0x85, ')', '=', '\b',	// 0x00-0x0E (é, è, ç, à in CP437)
	'\t', 'a', 'z', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '^', '$', '\n',			// 0x0F-0x1C
	0, 'q', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', 'm', 0x97, 0xFD,				// 0x1D-0x29 (ù, ²)
	0, '*', 'w', 'x', 'c', 'v', 'b', 'n', ',', ';', ':', '!', 0,					// 0x2A-0x36
	'*', 0, ' ', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,									// 0x37-0x44
	0, 0, '7', '8', '9', '-', '4', '5', '6', '+', '1', '2', '3', '0', '.',			// 0x45-0x53
	0, 0, '<', 0, 0																	// 0x54-0x58 ('<' en 0x56)
};

static const char layout_azerty_upper[] = {
	0, 0, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', 0xF8, '+', '\b',		// 0x00-0x0E (°)
	'\t', 'A', 'Z', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', 0, 0x9C, '\n',			// 0x0F-0x1C (¨ = 0, £)
	0, 'Q', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', 'M', '%', 0,					// 0x1D-0x29
	0, 0xE6, 'W', 'X', 'C', 'V', 'B', 'N', '?', '.', '/', 0x15, 0,					// 0x2A-0x36 (µ, §)
	'*', 0, ' ', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,									// 0x37-0x44
	0, 0, '7', '8', '9', '-', '4', '5', '6', '+', '1', '2', '3', '0', '.',			// 0x45-0x53
	0, 0, '>', 0, 0																	// 0x54-0x58
};
// clang-format on

const char *layout_lower = layout_qwerty_lower;
const char *layout_upper = layout_qwerty_upper;

struct lay_map {
	const char *lay_name;
	const char *lay_lower;
	const char *lay_upper;
};

static const struct lay_map lay_map[] = {
	{ "qwerty", layout_qwerty_lower, layout_qwerty_upper },
	{ "azerty", layout_azerty_lower, layout_azerty_upper },
	{ NULL, NULL, NULL },
};

void change_layout(const char *lay_name)
{
	for (size_t i = 0; lay_map[i].lay_name; ++i) {
		if (strcmp(lay_name, lay_map[i].lay_name) == 0) {
			layout_lower = lay_map[i].lay_lower;
			layout_upper = lay_map[i].lay_upper;
		}
	}
}
