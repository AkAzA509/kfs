#include "test_support.h"
#include <core/multiboot.h>

/* Validate the Multiboot handoff provided by GRUB. */
void __kstart(unsigned long magic, unsigned long address)
{
	multiboot_info *info = (multiboot_info *)(uintptr_t)address;

	test_serial_init();
	test_serial_puts("TEST_START multiboot\n");

	if (magic != MULTIBOOT_BOOTLOADER_MAGIC) {
		test_serial_puts("TEST_FAIL invalid-multiboot-magic\n");
		test_exit(0x11);
	}

	if (address == 0 || (address & 0x3) != 0 || info->flags == 0) {
		test_serial_puts("TEST_FAIL multiboot-pointer\n");
		test_exit(0x12);
	}

	if ((info->flags & MULTIBOOT_INFO_AOUT_SYMS) &&
	    (info->flags & MULTIBOOT_INFO_ELF_SHDR)) {
		test_serial_puts("TEST_FAIL multiboot-symbol-flags\n");
		test_exit(0x13);
	}

	if ((info->flags & MULTIBOOT_INFO_ELF_SHDR) &&
	    (info->u.elf_sec.num == 0 || info->u.elf_sec.size == 0 ||
	     info->u.elf_sec.addr == 0)) {
		test_serial_puts("TEST_FAIL multiboot-elf-sections\n");
		test_exit(0x14);
	}

	test_serial_puts("TEST_PASS multiboot\n");
	test_serial_puts("TEST_RUN_COMPLETE PASS\n");
	test_exit(0x10);
}
