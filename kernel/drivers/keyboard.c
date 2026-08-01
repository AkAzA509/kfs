#include <drivers/keyboard.h>
#include <drivers/console.h>
#include <kernel/common.h>
#include <kernel/shell.h>
#include <kernel/log.h>
#include <kernel/tty.h>
#include <kernel/io.h>
#include <stdbool.h>
#include <stdio.h>

// Single scancode
#define LEFT_SHIFT 0x2A
#define RIGHT_SHIFT 0x36
#define LEFT_CTRL 0x1D

#define CAPS_LOCK 0x3A
#define HOME 0x47
#define END 0x4F

#define RELEASE_MSK 0x80
#define CODE_MSK 0x7F
#define EXTEND_CODE 0xE0

// Double scancode 0xE0 + ...
#define RIGHT_CTRL 0x1D
#define DELETE 0x53

#define ARROW_UP 0x48
#define ARROW_DOWN 0x50
#define ARROW_LEFT 0x4B
#define ARROW_RIGHT 0x4D

#define PAGE_UP 0x49
#define PAGE_DOWN 0x51

// clang-format off
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
// clang-format on

static bool shift = false;
static bool caps_lock = false;
static bool extended_pending = false;

static void handle_screen_switch(u8_t code)
{
	switch (code) {
	case 0x3b:
		screen_switch(0);
		return; // f1
	case 0x3c:
		screen_switch(1);
		return; // f2
	case 0x3d:
		screen_switch(2);
		return; // f3
	case 0x3e:
		screen_switch(3);
		return; // f4
	default:
		break;
	}
}

// Fake exit, to modify when the kernel has memory
void handle_exit(void)
{
	printf("Shuting down ...\n");
	SHUTDOWN;
}

static void handle_scroll_key(u8_t code, bool is_release)
{
	if (is_release)
		return;

	switch (code) {
	case ARROW_UP:
		screen_scroll(-1);
		break;
	case ARROW_DOWN:
		screen_scroll(1);
		break;
	case PAGE_UP:
		screen_scroll(-(int)g_screen.total_rows);
		break;
	case PAGE_DOWN:
		screen_scroll((int)g_screen.total_rows);
		break;
	case HOME:
		screen_snap();
		break;
	default:
		break;
	}
}

static void handle_edit_key(u8_t code, bool is_release)
{
	if (is_release)
		return;

	switch (code) {
	case ARROW_LEFT:
		move_cursor(move_one_left);
		break;
	case ARROW_RIGHT:
		move_cursor(move_one_right);
		break;
	case END:
		move_cursor(move_end);
		break;
	case 0x1E:
		move_cursor(move_start);
		break; // ctrl + a
	default:
		break;
	}
}

// For the key with 2 code
static void handle_extended_key(u8_t code, bool is_release)
{
	// klog("in the special key code : %#.2x\n", code);
	if (code >= 0x47 && code <= 0x51) // HOME to PAGE DOWN
		handle_scroll_key(code, is_release);
	if (code == ARROW_LEFT || code == ARROW_RIGHT || code == END ||
	    code == 0x1E) {
		// klog("in handler\n");
		handle_edit_key(code, is_release);
	}
	if (code == DELETE && !is_release)
		editor_delete();
	if (code == 0x26 && !is_release) {
		screen_clear();
		print_prompt();
	}
}

static void handle_release_special_key(u8_t code)
{
	u8_t key = code & ~RELEASE_MSK;

	if (key == LEFT_SHIFT || key == RIGHT_SHIFT)
		shift = false;
	return;
}

static void handle_press_special_key(u8_t code)
{
	if (code == LEFT_SHIFT || code == RIGHT_SHIFT) {
		shift = true;
		return;
	}
	if (code == CAPS_LOCK)
		caps_lock = !caps_lock;
}

static bool handle_single_key(u8_t code, bool is_release)
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
		editor_backspace();
		return false;
	}
	return true;
}

static void print_code(u8_t code)
{
	char val = keycode[code];
	if (!val)
		return;

	if (shift && !caps_lock)
		val = keycode_shift[code];
	else if ((caps_lock && shift && (val < 'a' || val > 'z')) ||
		 (caps_lock && !shift && val >= 'a' && val <= 'z'))
		val = keycode_shift[code];

	if (!line_visible(&g_screens[current_screen],
			  g_screens[current_screen].head))
		screen_snap();
	editor_putchar(val);
}

static void read_scancode(u8_t scancode)
{
#ifdef DEBUG
	// serial_print_hex(scancode);
#endif
	if (scancode == EXTEND_CODE || scancode == LEFT_CTRL ||
	    scancode == RIGHT_CTRL) {
		extended_pending = true;
		return;
	}

	bool is_extended = extended_pending;
	bool is_release = scancode & RELEASE_MSK;
	u8_t code = scancode & CODE_MSK;

	extended_pending = false;

	if (is_extended) {
		handle_extended_key(code, is_release);
		return;
	}
	if (!handle_single_key(code, is_release))
		return;

	print_code(code);
}

void keyboard_handler()
{
	while (1) {
		// 0x64 register give the port's status (ready or not, busy ...)
		if (inb(0x64) & 0x01) {
			// 0x60 register give the data (key press)
			u8_t scancode = inb(0x60);
			read_scancode(scancode);
		}
	}
}
