#include "test_support.h"

#define KERNEL_LOAD_ADDRESS 0x00200000
#define STACK_SIZE 16384

extern void _start(void);
extern u8_t stack_top;

/* Validate the addresses produced by the kernel linker script. */
void __kstart(unsigned long magic, unsigned long address)
{
	u32_t stack_pointer;
	uintptr_t entry = (uintptr_t)_start;
	uintptr_t stack_end = (uintptr_t)&stack_top;

	(void)address;
	test_serial_init();
	test_serial_puts("TEST_START layout\n");

	if (magic != MULTIBOOT_BOOTLOADER_MAGIC) {
		test_serial_puts("TEST_FAIL invalid-multiboot-magic\n");
		test_exit(0x11);
	}

	__asm__ volatile("mov %%esp, %0" : "=r"(stack_pointer));
	if (entry < KERNEL_LOAD_ADDRESS || (entry & 0xfff) != 0x30 ||
	    (stack_end & 0xf) != 0 || stack_end <= entry ||
	    stack_pointer < stack_end - STACK_SIZE ||
	    stack_pointer > stack_end) {
		test_serial_puts("TEST_FAIL layout\n");
		test_exit(0x12);
	}

	test_serial_puts("TEST_PASS layout\n");
	test_serial_puts("TEST_RUN_COMPLETE PASS\n");
	test_exit(0x10);
}
