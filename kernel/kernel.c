#include "../includes/stddef.h"
// #include "../includes/kernel.h"
#include "../includes/fonts.h"
#include "multiboot.h"
#include "kernel_internal.h"

/* Check if the compiler thinks you are targeting the wrong operating system. */
#if defined(__linux__)
#error "You are not using a cross-compiler, you will most certainly run into trouble"
#endif

/* Check for the 32-bit ix86 targets. */
#if !defined(__i386__)
#error "This code needs to be compiled with a ix86-elf compiler"
#endif

#define CHECK_FLAG(flags, bit)	((flags) & (1 << (bit)))

size_t	t_row;
size_t	t_column;
u8_t	t_color;
volatile u32_t*	t_buffer;

void init_term(void)
{
	t_row = 0;
	t_column = 0;
	
	PSF1_Header font_header = {
		.characterSize = font_data[3],
		.fontMode = font_data[2],
		.magic = (font_data[1] << 8) | font_data[0]
	};
	
	if (font_header.characterSize != 16)
		return ;

	// t_color = vga_entry_color(VGA_COLOR_GREEN, VGA_COLOR_BLACK);
	
	// for (size_t y = 0; y < VGA_HEIGHT; y++) {
		// for (size_t x = 0; x < VGA_WIDTH; x++) {
			// t_color = vga_entry_color(y % VGA_COLOR_END, (x + y) % VGA_COLOR_END);
			// const size_t index = y * VGA_WIDTH + x;
			// t_buffer[index] = vga_entry(' ', t_color);
			// send_char_to_vga(' ', t_color, x, y);
		// }
	// }
	// set_cursor(0, 0);
	for (size_t i = 0; i < MAX_COL * MAX_LINE * 2; ++i)
		*(t_buffer + i) = 0;
}

void kernel_main(unsigned long magic, unsigned long addr)
{
	multiboot_info	*mbi = (multiboot_info *)addr;
	
	t_buffer = (volatile u32_t *)(u32_t)mbi->framebuffer_addr;


	// outb(0x3F8, '0' + mbi->framebuffer_type);
	// kprint("vbe_mode type %x\n", mbi->vbe_mode);
	// kprint("type  %d\n", mbi->framebuffer_type);
	// kprint("height  %d\n", mbi->framebuffer_height);
	// kprint("width  %d\n", mbi->framebuffer_width);
	// kprint("bpp  %d\n", mbi->framebuffer_bpp);
	// kprint("pitch  %d\n", mbi->framebuffer_pitch);
	// kprint("addr  %p\n", mbi->framebuffer_addr);
	// init_term();
	
	// if (magic != MULTIBOOT_BOOTLOADER_MAGIC) {
	// 	kprint("Invalid magic number: multiboot error: %#x\n", (unsigned)magic);
	// 	return ;
	// }
	
	// kprint("magic = %#x\n", magic);
	
	// kprint("flags = %#x\n", mbi->flags);
	
	// if (CHECK_FLAG (mbi->flags, 0))
	// 	kprint("mem_lower = %zuKB, mem_upper = %zuKB\n", mbi->mem_lower, mbi->mem_upper);
	
	// if (CHECK_FLAG (mbi->flags, 1))
	// 	kprint("boot_device = %#x\n", mbi->boot_device);
		
	// if (CHECK_FLAG (mbi->flags, 2))
	// 	kprint("cmdline = %s\n", (char *) mbi->cmdline);
		
	// if (CHECK_FLAG (mbi->flags, 3)) {
	// 	multiboot_mod_list *mod = (multiboot_mod_list *) mbi->mods_addr;
		
	// 	kprint("mods_count = %d, mods_addr = %#x\n", (int)mbi->mods_count, (int)mbi->mods_addr);
	// 	for (size_t i = 0; i < mbi->mods_count; i++, mod++)
	// 		kprint(" mod_start = %#x, mod_end = %#x, cmdline = %s\n",
	// 				(unsigned) mod->mod_start,
	// 				(unsigned) mod->mod_end,
	// 				(char *) mod->cmdline);
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
	// 	multiboot_elf_section_header_table *multiboot_elf_sec = &(mbi->u.elf_sec);

	// 	kprint("multiboot_elf_sec: num = %u, size = %#x,"
	// 			" addr = %#x, shndx = %#x\n",
	// 			(unsigned) multiboot_elf_sec->num, (unsigned) multiboot_elf_sec->size,
	// 			(unsigned) multiboot_elf_sec->addr, (unsigned) multiboot_elf_sec->shndx);
	// }
	
	// if (CHECK_FLAG (mbi->flags, 6)) {
	// 	multiboot_mmap_entry *mmap = (multiboot_mmap_entry *) mbi->mmap_addr;
		
	// 	kprint("mmap_addr = %#x, mmap_length = %#x\n", (unsigned)mbi->mmap_addr, (unsigned)mbi->mmap_length);
	// 	for (; (unsigned long) mmap < mbi->mmap_addr + mbi->mmap_length; mmap = (multiboot_mmap_entry *) ((unsigned long) mmap + mmap->size + sizeof (mmap->size)))
	// 		kprint(" size = %#x, base_addr = %#x,"
	// 				" length = %#x, type = %#x\n",
	// 				(unsigned) mmap->size,
	// 				(unsigned) (mmap->addr >> 32),
	// 				(unsigned) (mmap->addr & 0xffffffff),
	// 				(unsigned) (mmap->len >> 32),
	// 				(unsigned) (mmap->len & 0xffffffff),
	// 				(unsigned) mmap->type);
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
				
	// 			palette = (struct multiboot_color *) mbi->framebuffer_palette_addr;

	// 			color = 0;
	// 			best_distance = 4*256*256;
				
	// 			for (i = 0; i < mbi->framebuffer_palette_num_colors; i++) {
	// 				distance = (0xff - palette[i].blue) * (0xff - palette[i].blue)
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
	// 			color = ((1 << mbi->framebuffer_blue_mask_size) - 1) << mbi->framebuffer_blue_field_position;
	// 			break;

	// 		case MULTIBOOT_FRAMEBUFFER_TYPE_EGA_TEXT:
	// 			color = '\\' | 0x0100;
	// 			break;

	// 		default:
	// 			color = 0xffffffff;
	// 			break;
	// 	}
	// 	for (i = 0; i < mbi->framebuffer_width && i < mbi->framebuffer_height; i++) {
	// 		switch (mbi->framebuffer_bpp) {
	// 			case 8:
	// 			{
	// 				u8_t *pixel = fb + mbi->framebuffer_pitch * i + i;
	// 				*pixel = color;
	// 			}
	// 			break;
	// 			case 15:
	// 			case 16:
	// 			{
	// 				u16_t *pixel = fb + mbi->framebuffer_pitch * i + 2 * i;
	// 				*pixel = color;
	// 			}
	// 			break;
	// 			case 24:
	// 			{
	// 				u32_t *pixel = fb + mbi->framebuffer_pitch * i + 3 * i;
	// 				*pixel = (color & 0xffffff) | (*pixel & 0xff000000);
	// 			}
	// 			break;

	// 			case 32:
	// 			{
	// 				u32_t *pixel = fb + mbi->framebuffer_pitch * i + 4 * i;
	// 				*pixel = color;
	// 			}
	// 			break;
	// 		}
	// 	}
	// }
	
	// keyboard_handler();
}