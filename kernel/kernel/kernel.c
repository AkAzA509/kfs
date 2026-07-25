#include <arch/i386/keyboard.h>
#include <kernel/multiboot.h>
#include <kernel/kernel.h>
#include <kernel/common.h>
#include <arch/i386/tty.h>
#include <kernel/init.h>
#include <kernel/log.h>
#include <stdbool.h>
#include <stdio.h>

/* Check if the compiler thinks you are targeting the wrong operating system. */
#if defined(__linux__)
	#error "You are not using a cross-compiler, you will most certainly run into trouble"
#endif

/* Check for the 32-bit ix86 targets. */
#if !defined(__i386__)
	#error "This code needs to be compiled with a ix86-elf compiler"
#endif

#define CHECK_FLAG(flags, bit) ((flags) & (1 << (bit)))

void serial_print_hex(u32_t val) {
	char hex[] = "0123456789abcdef";
	outb(0x3F8, '0');
	outb(0x3F8, 'x');
	for (int i = 7; i >= 0; i--)
		outb(0x3F8, hex[(val >> (i * 4)) & 0xF]);
	outb(0x3F8, '\n');
}

void	kmain(void) {
	// #ifdef DEBUG
	// 	#include <testing/testing.h>
	// 	test_screen();
	// 	debug_screen();
	// 	test_printf_run();
	// #endif // DEBUG

	// log_stack(4);
	// log_stack(0);
	printf("test %f\n", 43.34);
	printf("%%");
	printf("Hello world!\n");
	screen_switch(1);
	screen_switch(0);
	keyboard_handler();
}

static void	panic_print(char *str)
{
	char *vga = (char *)0xB8000;
	for (int i = 0; str[i]; i++) {
		vga[i * 2] = str[i];
		vga[i * 2 + 1] = 0x4F;
	}
}

static bool check_multiboot(multiboot_info *mbi, unsigned long magic) {
	if (magic != MULTIBOOT_BOOTLOADER_MAGIC) {
		panic_print("Invalid magic number: multiboot error");
		return false;
	}

	// if (mbi->flags & MULTIBOOT_INFO_FRAMEBUFFER_INFO && mbi->framebuffer_type != 1) {
	// 	panic_print("Invalid framebuffer flags: multiboot error");
	// 	return false;
	// }

	if (CHECK_FLAG (mbi->flags, 4) && CHECK_FLAG (mbi->flags, 5)) {
		panic_print("Both bits 4 and 5 are set\n");
		return false;
	}

	return true;
}

void	__kstart(unsigned long magic, unsigned long addr)
{
	multiboot_info *mbi = (multiboot_info *)addr;

	if (!check_multiboot(mbi, magic))
		HALT_ERROR;

	init_display(mbi);

	if (!init_term())
		HALT_ERROR;

	init_gdt();

	set_term_color(make_color(COLOR_LIGHT_MAGENTA, COLOR_WHITE));
	ASCII_LOGO;
	set_term_color(make_color(COLOR_WHITE, COLOR_BLACK));

	kmain();	/* if kmain return, that sould not happen but in case we hlt infinitly */
	HALT_ERROR;
}

// void multibootfunctest()
// {
// if (magic != MULTIBOOT_BOOTLOADER_MAGIC) {
// 	printf("Invalid magic number: multiboot error: %#x\n", (unsigned)magic);
// 	return ;
// }

// printf("magic = %#x\n", magic);

// printf("flags = %#x\n", mbi->flags);

// if (CHECK_FLAG (mbi->flags, 0))
// 	printf("mem_lower = %zuKB, mem_upper = %zuKB\n", mbi->mem_lower,
// mbi->mem_upper);

// if (CHECK_FLAG (mbi->flags, 1))
// 	printf("boot_device = %#x\n", mbi->boot_device);

// if (CHECK_FLAG (mbi->flags, 2))
// 	printf("cmdline = %s\n", (char *) mbi->cmdline);

// if (CHECK_FLAG (mbi->flags, 3)) {
// 	multiboot_mod_list *mod = (multiboot_mod_list *) mbi->mods_addr;

// 	printf("mods_count = %d, mods_addr = %#x\n", (int)mbi->mods_count,
// (int)mbi->mods_addr); 	for (size_t i = 0; i < mbi->mods_count; i++,
// mod++) 		printf(" mod_start = %#x, mod_end = %#x, cmdline =
// %s\n", 				(unsigned) mod->mod_start,
// (unsigned) mod->mod_end, 				(char *) mod->cmdline);
// }

// if (CHECK_FLAG (mbi->flags, 4) && CHECK_FLAG (mbi->flags, 5)) {
// 	printf("Both bits 4 and 5 are set.\n");
// 	return;
// }

// if (CHECK_FLAG (mbi->flags, 4)) {
// 	multiboot_aout_symbol_table *multiboot_aout_sym = &(mbi->u.aout_sym);

// 	printf("multiboot_aout_symbol_table: tabsize = %#0x, "
// 			"strsize = %#x, addr = %#x\n",
// 			(unsigned) multiboot_aout_sym->tabsize,
// 			(unsigned) multiboot_aout_sym->strsize,
// 			(unsigned) multiboot_aout_sym->addr);
// }

// if (CHECK_FLAG (mbi->flags, 5)) {
// 	multiboot_elf_section_header_table *multiboot_elf_sec =
// &(mbi->u.elf_sec);

// 	printf("multiboot_elf_sec: num = %u, size = %#x,"
// 			" addr = %#x, shndx = %#x\n",
// 			(unsigned) multiboot_elf_sec->num, (unsigned)
// multiboot_elf_sec->size, 			(unsigned)
// multiboot_elf_sec->addr, (unsigned) multiboot_elf_sec->shndx);
// }

// if (CHECK_FLAG (mbi->flags, 6)) {
// 	multiboot_mmap_entry *mmap = (multiboot_mmap_entry *) mbi->mmap_addr;

// 	printf("mmap_addr = %#x, mmap_length = %#x\n", (unsigned)mbi->mmap_addr,
// (unsigned)mbi->mmap_length); 	for (; (unsigned long) mmap <
// mbi->mmap_addr + mbi->mmap_length; mmap = (multiboot_mmap_entry *) ((unsigned
// long) mmap + mmap->size + sizeof (mmap->size))) 		printf(" size =
// %#x, base_addr = %#x," 				" length = %#x, type =
// %#x\n", 				(unsigned) mmap->size,
// (unsigned) (mmap->addr >> 32), 				(unsigned)
// (mmap->addr & 0xffffffff), 				(unsigned) (mmap->len >>
// 32), 				(unsigned) (mmap->len & 0xffffffff),
// (unsigned) mmap->type);
// }

// if (CHECK_FLAG (mbi->flags, 12)) {
// 	u32_t color;
// 	unsigned i;
// 	void *fb = (void *) (unsigned long) mbi->framebuffer_addr;

// 	switch (mbi->framebuffer_type) {
// 		case MULTIBOOT_FRAMEBUFFER_TYPE_INDEXED:
// 		{
// 			unsigned best_distance, distance;
// 			struct multiboot_color *palette;

// 			palette = (struct multiboot_color *)
// mbi->framebuffer_palette_addr;

// 			color = 0;
// 			best_distance = 4*256*256;

// 			for (i = 0; i < mbi->framebuffer_palette_num_colors;
// i++) { 				distance = (0xff - palette[i].blue) *
// (0xff - palette[i].blue)
// 				+ palette[i].red * palette[i].red
// 				+ palette[i].green * palette[i].green;
// 				if (distance < best_distance) {
// 					color = i;
// 					best_distance = distance;
// 				}
// 			}
// 		}
// 			break;

// 		case MULTIBOOT_FRAMEBUFFER_TYPE_RGB:
// 			color = ((1 << mbi->framebuffer_blue_mask_size) - 1) <<
// mbi->framebuffer_blue_field_position; 			break;

// 		case MULTIBOOT_FRAMEBUFFER_TYPE_EGA_TEXT:
// 			color = '\\' | 0x0100;
// 			break;

// 		default:
// 			color = 0xffffffff;
// 			break;
// 	}
// 	for (i = 0; i < mbi->framebuffer_width && i < mbi->framebuffer_height;
// i++) { 		switch (mbi->framebuffer_bpp) {
// case 8:
// 			{
// 				u8_t *pixel = fb + mbi->framebuffer_pitch * i +
// i; 				*pixel = color;
// 			}
// 			break;
// 			case 15:
// 			case 16:
// 			{
// 				u16_t *pixel = fb + mbi->framebuffer_pitch * i +
// 2 * i; 				*pixel = color;
// 			}
// 			break;
// 			case 24:
// 			{
// 				u32_t *pixel = fb + mbi->framebuffer_pitch * i +
// 3 * i; 				*pixel = (color & 0xffffff) | (*pixel &
// 0xff000000);
// 			}
// 			break;

// 			case 32:
// 			{
// 				u32_t *pixel = fb + mbi->framebuffer_pitch * i +
// 4 * i; 				*pixel = color;
// 			}
// 			break;
// 		}
// 	}
// }
// }
