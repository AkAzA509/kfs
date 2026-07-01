#ifndef KERNEL_INTERNAL_H
#define KERNEL_INTERNAL_H

#include "../includes/stdint.h"
#include "../include/stddef.h"

// terminal size
#define MAX_COL		80
#define MAX_LINE	24
#define SCREEN		0xB8000
#define ATTRIBUTE	7

extern size_t	t_row;
extern size_t	t_column;
extern u8_t		t_color;
extern volatile u32_t*	t_buffer;

// Hardware text mode color constants
enum vga_color {
	VGA_COLOR_BLACK = 0,
	VGA_COLOR_BLUE = 1,
	VGA_COLOR_GREEN = 2,
	VGA_COLOR_CYAN = 3,
	VGA_COLOR_RED = 4,
	VGA_COLOR_MAGENTA = 5,
	VGA_COLOR_BROWN = 6,
	VGA_COLOR_LIGHT_GREY = 7,
	VGA_COLOR_DARK_GREY = 8,
	VGA_COLOR_LIGHT_BLUE = 9,
	VGA_COLOR_LIGHT_GREEN = 10,
	VGA_COLOR_LIGHT_CYAN = 11,
	VGA_COLOR_LIGHT_RED = 12,
	VGA_COLOR_LIGHT_MAGENTA = 13,
	VGA_COLOR_LIGHT_BROWN = 14,
	VGA_COLOR_WHITE = 15,
	VGA_COLOR_END
};

// typedef struct multiboot_info {
// 	u32_t				flags;					// offset 0  - quels champs sont valides
// 	u32_t				mem_lower;				// offset 4
// 	u32_t				mem_upper;				// offset 8
// 	u32_t				boot_device;			// offset 12
// 	u32_t				cmdline;				// offset 16
// 	u32_t				mods_count;				// offset 20
// 	u32_t				mods_addr;				// offset 24
// 	u32_t				syms[4];				// offset 28
// 	u32_t				mmap_length;			// offset 44
// 	u32_t				mmap_addr;				// offset 48
// 	u32_t				drives_length;			// offset 52
// 	u32_t				drives_addr;			// offset 56
// 	u32_t				config_table;			// offset 60
// 	u32_t				boot_loader_name;		// offset 64
// 	u32_t				apm_table;				// offset 68
// 	u32_t				vbe_control_info;		// offset 72
// 	u32_t				vbe_mode_info;			// offset 76
// 	u16_t				vbe_mode;				// offset 80
// 	u16_t				vbe_interface_seg;		// offset 82
// 	u16_t				vbe_interface_off;		// offset 84
// 	u16_t				vbe_interface_len;		// offset 86
// 	unsigned long long	framebuffer_addr;		// offset 88 - 64 bits !
// 	u32_t				framebuffer_pitch;		// offset 96
// 	u32_t				framebuffer_width;		// offset 100
// 	u32_t				framebuffer_height;		// offset 104
// 	u8_t				framebuffer_bpp;		// offset 108
// 	u8_t				framebuffer_type;		// offset 109
// }						multiboot_info_t;

// Buffer and color

u8_t	vga_entry_color(enum vga_color fg, enum vga_color bg);
void	send_char_to_vga(char c, u8_t color, size_t x, size_t y);
u16_t	vga_entry(unsigned char uc, u8_t color);
void	set_term_color(u8_t color);
void set_cursor(int x, int y);

// I/O

void outb(u16_t port, u8_t val);
u8_t inb(u16_t port);

#endif // KERNEL_INTERNAL_H