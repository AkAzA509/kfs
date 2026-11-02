#include "test_support.h"
#include <core/init.h>
#include <drivers/console.h>

#define GDT_ENTRY_SIZE 8
#define GDT_ENTRY_COUNT 7

struct [[gnu::packed]] gdt_register {
	u16_t limit;
	u32_t address;
};

/* Return the descriptor access byte at a GDT index. */
static u8_t gdt_access(const struct gdt_register *gdtr, u32_t index)
{
	const u8_t *entry = (const u8_t *)(uintptr_t)gdtr->address;

	return entry[index * GDT_ENTRY_SIZE + 5];
}

/* Keep the real GDT implementation independent from the console in this test. */
void set_term_color(u8_t color)
{
	(void)color;
}

u8_t make_color(t_color foreground, t_color background)
{
	return (u8_t)(foreground | (background << 4));
}

int printf(const char *format, ...)
{
	(void)format;
	return 0;
}

void __kstart(unsigned long magic, unsigned long address)
{
	struct gdt_register gdtr;

	(void)address;
	test_serial_init();
	test_serial_puts("TEST_START gdt\n");

	if (magic != MULTIBOOT_BOOTLOADER_MAGIC) {
		test_serial_puts("TEST_FAIL invalid-multiboot-magic\n");
		test_exit(0x11);
	}

	init_gdt();
	__asm__ volatile("sgdt %0" : "=m"(gdtr));

	if (gdtr.limit != GDT_ENTRY_SIZE * GDT_ENTRY_COUNT - 1) {
		test_serial_puts("TEST_FAIL gdt-limit\n");
		test_exit(0x12);
	}

	if (gdtr.address == 0 || gdt_access(&gdtr, 1) != 0x9a ||
	    gdt_access(&gdtr, 2) != 0x93 || gdt_access(&gdtr, 3) != 0x93 ||
	    gdt_access(&gdtr, 4) != 0xfa || gdt_access(&gdtr, 5) != 0xf2 ||
	    gdt_access(&gdtr, 6) != 0xf2) {
		test_serial_puts("TEST_FAIL gdt-descriptors\n");
		test_exit(0x13);
	}

	test_serial_puts("TEST_PASS gdt\n");
	test_serial_puts("TEST_RUN_COMPLETE PASS\n");
	test_exit(0x10);
}
