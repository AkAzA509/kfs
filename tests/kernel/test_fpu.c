#include "test_support.h"

/* Validate the FPU state prepared by the bootloader. */
void __kstart(unsigned long magic, unsigned long address)
{
	u32_t cr0;
	u16_t control_word;
	u64_t left = 0x3ff8000000000000;
	u64_t right = 0x4002000000000000;
	u64_t result;

	(void)address;
	test_serial_init();
	test_serial_puts("TEST_START fpu\n");

	if (magic != MULTIBOOT_BOOTLOADER_MAGIC) {
		test_serial_puts("TEST_FAIL invalid-multiboot-magic\n");
		test_exit(0x11);
	}

	__asm__ volatile("mov %%cr0, %0" : "=r"(cr0));
	if ((cr0 & (1 << 2)) != 0 || (cr0 & (1 << 1)) == 0 ||
	    (cr0 & (1 << 5)) == 0) {
		test_serial_puts("TEST_FAIL fpu-control\n");
		test_exit(0x12);
	}

	__asm__ volatile("fnstcw %0" : "=m"(control_word));
	if (control_word != 0x037f) {
		test_serial_puts("TEST_FAIL fpu-control-word\n");
		test_exit(0x13);
	}

	__asm__ volatile("fldl %1\n\t"
			 "faddl %2\n\t"
			 "fstpl %0"
			 : "=m"(result)
			 : "m"(left), "m"(right)
			 : "st");
	if (result != 0x400e000000000000) {
		test_serial_puts("TEST_FAIL fpu-operation\n");
		test_exit(0x14);
	}

	test_serial_puts("TEST_PASS fpu\n");
	test_serial_puts("TEST_RUN_COMPLETE PASS\n");
	test_exit(0x10);
}
