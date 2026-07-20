#include <kernel/kprint/kprint.h>
#include <arch/i386/keyboard.h>
#include <kernel/multiboot.h>
#include <kernel/kernel.h>
#include <arch/i386/tty.h>
#include <kernel/init.h>
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

static bool init_ctx(multiboot_info *mbi, unsigned long magic) {
	if (magic != MULTIBOOT_BOOTLOADER_MAGIC) {
		kprint("Invalid magic number: multiboot error: %#x\n", (unsigned)magic);
		return false;
	}

	init_display(mbi);

	if (!init_term())
		return false;

	set_term_color(make_color(COLOR_LIGHT_RED, COLOR_BLACK));
	kprint(BOOT_LOG "terminal initialized\n");
	set_term_color(make_color(COLOR_WHITE, COLOR_BLACK));

	current_driver->clear();

	set_term_color(make_color(COLOR_LIGHT_MAGENTA, COLOR_WHITE));
	kprint(" _____    _     ___      \n");
	kprint("|_   _|__| | __/ _ \\ ___ \n");
	kprint("  | |/ _ \\ |/ / | | / __|\n");
	kprint("  | |  __/   <| |_| \\__ \\\n");
	kprint("  |_|\\___|_|\\_\\\\___/|___/\n");
	kprint("_________________________\n");
	set_term_color(make_color(COLOR_WHITE, COLOR_BLACK));
	#ifdef DEBUG
		debug_diplay();
	#endif
	return true;
}

// #define DEBUG

#ifdef DEBUG
#include <kernel/framebuffer.h>
void screen_test() {
	kprint("Test screen 0\n\n");

	screen_switch(1);
	char buffer[100] = "012345678910111213141516171819202122232425\0";
	kprint("test screen 1 %s\n\n", buffer);

	screen_switch(2);
	kprint("Test backspace screen 3\n il ne dois rien y avaoir apres ca :%s",
		buffer);
	for (size_t i = 0; buffer[i]; ++i) {
		backspace();
	}
	screen_switch(3);
	debug_diplay();
	kprint("coucou after display\n\n\n\n\n\n\nplus bas");
	backspace();
	backspace();
	backspace();
	backspace();
	backspace();
	backspace();
	backspace();
	backspace();

	screen_switch(0);
	const u32_t cols = g_screen.width / 8;
	const u32_t rows = g_screen.height / font_header.charsize;
	set_term_color(make_color(COLOR_CYAN, COLOR_WHITE));
	for (size_t i = 0; i < cols * rows - 1; ++i)
		kprint("0");
	set_term_color(make_color(COLOR_WHITE, COLOR_BLACK));
}
#endif // DEBUG

void kernel_main(unsigned long magic, unsigned long addr) {
	multiboot_info *mbi = (multiboot_info *)addr;

	if (!init_ctx(mbi, magic))
		return;

	init_gdt();
	#ifdef DEBUG
		screen_test();
	#endif // DEBUG

	// kprint("test %zu\n", mbi->boot_loader_name);
	printf("test % #0-+coucou\n");
	
	// keyboard_handler();
}

// void multibootfunctest()
// {
// if (magic != MULTIBOOT_BOOTLOADER_MAGIC) {
// 	kprint("Invalid magic number: multiboot error: %#x\n", (unsigned)magic);
// 	return ;
// }

// kprint("magic = %#x\n", magic);

// kprint("flags = %#x\n", mbi->flags);

// if (CHECK_FLAG (mbi->flags, 0))
// 	kprint("mem_lower = %zuKB, mem_upper = %zuKB\n", mbi->mem_lower,
// mbi->mem_upper);

// if (CHECK_FLAG (mbi->flags, 1))
// 	kprint("boot_device = %#x\n", mbi->boot_device);

// if (CHECK_FLAG (mbi->flags, 2))
// 	kprint("cmdline = %s\n", (char *) mbi->cmdline);

// if (CHECK_FLAG (mbi->flags, 3)) {
// 	multiboot_mod_list *mod = (multiboot_mod_list *) mbi->mods_addr;

// 	kprint("mods_count = %d, mods_addr = %#x\n", (int)mbi->mods_count,
// (int)mbi->mods_addr); 	for (size_t i = 0; i < mbi->mods_count; i++,
// mod++) 		kprint(" mod_start = %#x, mod_end = %#x, cmdline =
// %s\n", 				(unsigned) mod->mod_start,
// (unsigned) mod->mod_end, 				(char *) mod->cmdline);
// }

// if (CHECK_FLAG (mbi->flags, 4) && CHECK_FLAG (mbi->flags, 5)) {
// 	kprint("Both bits 4 and 5 are set.\n");
// 	return;
// }

// if (CHECK_FLAG (mbi->flags, 4)) {
// 	multiboot_aout_symbol_table *multiboot_aout_sym = &(mbi->u.aout_sym);

// 	kprint("multiboot_aout_symbol_table: tabsize = %#0x, "
// 			"strsize = %#x, addr = %#x\n",
// 			(unsigned) multiboot_aout_sym->tabsize,
// 			(unsigned) multiboot_aout_sym->strsize,
// 			(unsigned) multiboot_aout_sym->addr);
// }

// if (CHECK_FLAG (mbi->flags, 5)) {
// 	multiboot_elf_section_header_table *multiboot_elf_sec =
// &(mbi->u.elf_sec);

// 	kprint("multiboot_elf_sec: num = %u, size = %#x,"
// 			" addr = %#x, shndx = %#x\n",
// 			(unsigned) multiboot_elf_sec->num, (unsigned)
// multiboot_elf_sec->size, 			(unsigned)
// multiboot_elf_sec->addr, (unsigned) multiboot_elf_sec->shndx);
// }

// if (CHECK_FLAG (mbi->flags, 6)) {
// 	multiboot_mmap_entry *mmap = (multiboot_mmap_entry *) mbi->mmap_addr;

// 	kprint("mmap_addr = %#x, mmap_length = %#x\n", (unsigned)mbi->mmap_addr,
// (unsigned)mbi->mmap_length); 	for (; (unsigned long) mmap <
// mbi->mmap_addr + mbi->mmap_length; mmap = (multiboot_mmap_entry *) ((unsigned
// long) mmap + mmap->size + sizeof (mmap->size))) 		kprint(" size =
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
