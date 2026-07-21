#include <stdio.h>
#include <kernel/kernel.h>
#include <arch/i386/tty.h>
#include <stdint.h>

struct s_gdt_entry {
	u16_t limit_1;	// limit, bits 0..15
	u16_t base_1;	// base, bits 0..15
	u8_t base_2;	// base, bits 16..23
	u8_t access;	// access info (type, data/code)
	u8_t lim_attr;	// bits 0..3: limit, bits 16..19, bits 4..7: additional data/code attributes
	u8_t base_3;	// base, bits 24..31
} __attribute__((packed));

struct s_gdt_addr {
	u16_t	limit;
	u32_t	addr;
} __attribute__((packed));

#define GDT_ENTRIES 7

static struct s_gdt_entry	gdt[GDT_ENTRIES];
static struct s_gdt_addr	gdt_addr;

extern void	update_gdt(u32_t gdt);

// flags = CF = 1100 1111
//		& F0  = 1100 0000

// limit = FFFFF = 1111 1111 1111 1111 1111
// 		& FFFF= 0000 1111 1111 1111 1111

// limit = FFFFF = 1111 1111 1111 1111 1111
// 		   >> 16 = 0000 0000 0000 0000 1111 | "1111 1111 1111 1111"
// 			& F  = 1111

// base = FFFFFFFF = 1111 1111 1111 1111 1111 1111 1111 1111
// 			& FFFF = 0000 0000 0000 0000 1111 1111 1111 1111

// base = FFFFFFFF = 1111 1111 1111 1111 1111 1111 1111 1111
// 			>> 16  = 0000 0000 0000 0000 1111 1111 1111 1111 | "1111 1111 1111 1111"
// 			& FF   = 0000 0000 0000 0000 0000 0000 1111 1111

// base = FFFFFFFF = 1111 1111 1111 1111 1111 1111 1111 1111
// 			>> 24  = 0000 0000 0000 0000 0000 0000 1111 1111 | "1111 1111 1111 1111 1111 1111"
// 			& FF   = 0000 0000 0000 0000 0000 0000 1111 1111

static void	create_gdt_entry(u8_t idx, u32_t base, u32_t limit, u8_t access, u8_t flags)
{
	gdt[idx].base_1 = base & 0xFFFF;			// the 16 low bits
	gdt[idx].base_2 = (base >> 16) & 0xFF;		// the 8 next bits
	gdt[idx].base_3 = (base >> 24) & 0xFF;		// the 8 last high bits

	gdt[idx].limit_1 = limit & 0xFFFF;			// the 16 low bits
	gdt[idx].lim_attr = (limit >> 16) & 0xF;	// the 4 high bits in the first bits of lim_attr
	gdt[idx].lim_attr |= flags & 0xF0;			// set the high nibble of flags

	gdt[idx].access = access;
}

void	init_gdt()
{
	gdt_addr.addr = (u32_t)&gdt;
	gdt_addr.limit = (sizeof(struct s_gdt_entry) * GDT_ENTRIES) - 1;

	create_gdt_entry(0, 0, 0, 0, 0); // NULL descriptor
	create_gdt_entry(1, 0, 0xFFFFF, 0x9A, 0xCF); // Kernel code
	create_gdt_entry(2, 0, 0xFFFFF, 0x92, 0xCF); // Kernel data
	create_gdt_entry(3, 0, 0xFFFFF, 0x92, 0xCF); // Kernel stack
	create_gdt_entry(4, 0, 0xFFFFF, 0xFA, 0xCF); // User code
	create_gdt_entry(5, 0, 0xFFFFF, 0xF2, 0xCF); // User data
	create_gdt_entry(6, 0, 0xFFFFF, 0xF2, 0xCF); // User stack

	// update_gdt((u32_t)&gdt);

	set_term_color(make_color(COLOR_LIGHT_RED, COLOR_BLACK));
	printf(BOOT_LOG "gdt initialized\n");
	set_term_color(make_color(COLOR_WHITE, COLOR_BLACK));
}


// 0x92 = 1001 0010 kernel data, stack

// Bit 7 (P)     = 1  → présent
// Bit 6-5 (DPL) = 00 → ring 0
// Bit 4 (S)     = 1  → segment normal
// Bit 3 (E)     = 0  → data
// Bit 2 (DC)    = 0  → expand-up
// Bit 1 (W)     = 1  → writable
// Bit 0 (A)     = 0  → pas encore accédé

// 0x9A = 1001 1010 kernel code

// Bit 7 (P)     = 1  → présent
// Bit 6-5 (DPL) = 00 → ring 0
// Bit 4 (S)     = 1  → segment normal
// Bit 3 (E)     = 1  → code
// Bit 2 (C)     = 0  → non-conforming
// Bit 1 (R)     = 1  → lisible
// Bit 0 (A)     = 0  → pas encore accédé

// // 0xF2 = 1001 0010 user data, stack

// Bit 7 (P)     = 1  → présent
// Bit 6-5 (DPL) = 11 → ring 3
// Bit 4 (S)     = 1  → segment normal
// Bit 3 (E)     = 0  → data
// Bit 2 (DC)    = 0  → expand-up
// Bit 1 (W)     = 1  → writable
// Bit 0 (A)     = 0  → pas encore accédé

// 0xFA = 1001 1010 user code

// Bit 7 (P)     = 1  → présent
// Bit 6-5 (DPL) = 11 → ring 3
// Bit 4 (S)     = 1  → segment normal
// Bit 3 (E)     = 1  → code
// Bit 2 (C)     = 0  → non-conforming
// Bit 1 (R)     = 1  → lisible
// Bit 0 (A)     = 0  → pas encore accédé