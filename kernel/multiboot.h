#ifndef MULTIBOOT_H
#define MULTIBOOT_H

#include "../includes/stdint.h"

/* How many bytes from the start of the file we search for the header. */
#define MULTIBOOT_SEARCH                        8192
#define MULTIBOOT_HEADER_ALIGN                  4

/* The magic field should contain this. */
#define MULTIBOOT_HEADER_MAGIC                  0x1BADB002

/* This should be in %eax. */
#define MULTIBOOT_BOOTLOADER_MAGIC              0x2BADB002

/* Alignment of multiboot modules. */
#define MULTIBOOT_MOD_ALIGN                     0x00001000

/* Alignment of the multiboot info structure. */
#define MULTIBOOT_INFO_ALIGN                    0x00000004

/* Flags set in the ’flags’ member of the multiboot header. */

/* Align all boot modules on i386 page (4KB) boundaries. */
#define MULTIBOOT_PAGE_ALIGN                    0x00000001

/* Must pass memory information to OS. */
#define MULTIBOOT_MEMORY_INFO                   0x00000002

/* Must pass video information to OS. */
#define MULTIBOOT_VIDEO_MODE                    0x00000004

/* This flag indicates the use of the address fields in the header. */
#define MULTIBOOT_AOUT_KLUDGE                   0x00010000

/* Flags to be set in the ’flags’ member of the multiboot info structure. */

/* is there basic lower/upper memory information? */
#define MULTIBOOT_INFO_MEMORY                   0x00000001
/* is there a boot device set? */
#define MULTIBOOT_INFO_BOOTDEV                  0x00000002
/* is the command-line defined? */
#define MULTIBOOT_INFO_CMDLINE                  0x00000004
/* are there modules to do something with? */
#define MULTIBOOT_INFO_MODS                     0x00000008

/* These next two are mutually exclusive */

/* is there a symbol table loaded? */
#define MULTIBOOT_INFO_AOUT_SYMS                0x00000010
/* is there an ELF section header table? */
#define MULTIBOOT_INFO_ELF_SHDR                 0X00000020

/* is there a full memory map? */
#define MULTIBOOT_INFO_MEM_MAP                  0x00000040

/* Is there drive info? */
#define MULTIBOOT_INFO_DRIVE_INFO               0x00000080

/* Is there a config table? */
#define MULTIBOOT_INFO_CONFIG_TABLE             0x00000100

/* Is there a boot loader name? */
#define MULTIBOOT_INFO_BOOT_LOADER_NAME         0x00000200

/* Is there a APM table? */
#define MULTIBOOT_INFO_APM_TABLE                0x00000400

/* Is there video information? */
#define MULTIBOOT_INFO_VBE_INFO                 0x00000800
#define MULTIBOOT_INFO_FRAMEBUFFER_INFO         0x00001000

struct multiboot_header {
	/* Must be MULTIBOOT_MAGIC - see above. */
	u32_t	magic;

	/* Feature flags. */
	u32_t	flags;

	/* The above fields plus this one must equal 0 mod 2^32. */
	u32_t	checksum;

	/* These are only valid if MULTIBOOT_AOUT_KLUDGE is set. */
	u32_t	header_addr;
	u32_t	load_addr;
	u32_t	load_end_addr;
	u32_t	bss_end_addr;
	u32_t	entry_addr;

  /* These are only valid if MULTIBOOT_VIDEO_MODE is set. */
	u32_t	mode_type;
	u32_t	width;
	u32_t	height;
	u32_t	depth;
};

/* The symbol table for a.out. */
typedef struct t_multiboot_aout_symbol_table {
	u32_t	tabsize;
	u32_t	strsize;
	u32_t	addr;
	u32_t	reserved;
}			multiboot_aout_symbol_table;

/* The section header table for ELF. */
typedef struct t_multiboot_elf_section_header_table {
	u32_t	num;
	u32_t	size;
	u32_t	addr;
	u32_t	shndx;
}			multiboot_elf_section_header_table;

typedef struct t_multiboot_info {
	/* Multiboot info version number */
	u32_t	flags;

	/* Available memory from BIOS */
	u32_t	mem_lower;
	u32_t	mem_upper;

	/* "root" partition */
	u32_t	boot_device;

	/* Kernel command line */
	u32_t	cmdline;

	/* Boot-Module list	*/
	u32_t	mods_count;
	u32_t	mods_addr;

	union
	{
		multiboot_aout_symbol_table			aout_sym;
		multiboot_elf_section_header_table	elf_sec;
	} u;

	/* Memory Mapping buffer */
	u32_t	mmap_length;
	u32_t	mmap_addr;

	/* Drive Info buffer */
	u32_t	drives_length;
	u32_t	drives_addr;

	/* ROM configuration table */
	u32_t	config_table;

	/* Boot	Loader Name */
	u32_t	boot_loader_name;

	/* APM table */
	u32_t	apm_table;

	/* Video */
	u32_t	vbe_control_info;
	u32_t	vbe_mode_info;
	u16_t	vbe_mode;
	u16_t	vbe_interface_seg;
	u16_t	vbe_interface_off;
	u16_t	vbe_interface_len;

	u64_t	framebuffer_addr;
	u32_t	framebuffer_pitch;
	u32_t	framebuffer_width;
	u32_t	framebuffer_height;
	u8_t	framebuffer_bpp;
#define MULTIBOOT_FRAMEBUFFER_TYPE_INDEXED	0
#define MULTIBOOT_FRAMEBUFFER_TYPE_RGB		1
#define MULTIBOOT_FRAMEBUFFER_TYPE_EGA_TEXT	2
	u8_t	framebuffer_type;
	union
	{
		struct
		{
			u32_t	framebuffer_palette_addr;
			u16_t	framebuffer_palette_num_colors;
		};
		struct
		{
			u8_t	framebuffer_red_field_position;
			u8_t	framebuffer_red_mask_size;
			u8_t	framebuffer_green_field_position;
			u8_t	framebuffer_green_mask_size;
			u8_t	framebuffer_blue_field_position;
			u8_t	framebuffer_blue_mask_size;
		};
	};
}			multiboot_info;

struct multiboot_color
{
	u8_t	red;
	u8_t	green;
	u8_t	blue;
};

typedef struct t_multiboot_mmap_entry {
	u32_t	size;
	u64_t	addr;
	u64_t	len;
#define MULTIBOOT_MEMORY_AVAILABLE				1
#define MULTIBOOT_MEMORY_RESERVED				2
#define MULTIBOOT_MEMORY_ACPI_RECLAIMABLE		3
#define MULTIBOOT_MEMORY_NVS					4
#define MULTIBOOT_MEMORY_BADRAM					5
	u32_t	type;
} __attribute__((packed)) multiboot_mmap_entry;
// typedef struct  multiboot_memory_map_t;

typedef struct t_multiboot_mod_list {
	/* the memory used goes from bytes ’mod_start’ to ’mod_end-1’ inclusive */
	u32_t	mod_start;
	u32_t	mod_end;

	/* Module command line */
	u32_t	cmdline;

	/* padding to take it to 16 bytes (must be zero) */
	u32_t	pad;
}			multiboot_mod_list;

/* APM BIOS info. */
struct multiboot_apm_info
{
	u16_t	version;
	u16_t	cseg;
	u32_t	offset;
	u16_t	cseg_16;
	u16_t	dseg;
	u16_t	flags;
	u16_t	cseg_len;
	u16_t	cseg_16_len;
	u16_t	dseg_len;
};

#endif // MULTIBOOT_H