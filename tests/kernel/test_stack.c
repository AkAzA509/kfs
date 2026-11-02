#include "test_support.h"

/* Exercise the stack installed by the bootloader with nested calls. */
static __attribute__((noinline)) bool stack_probe(u32_t depth, uintptr_t parent)
{
	volatile u32_t guard[64];
	uintptr_t current = (uintptr_t)&guard[0];

	for (u32_t index = 0; index < 64; index++)
		guard[index] = 0xa5000000 | index;

	if (parent != 0 && current >= parent)
		return false;

	if (depth != 0 && !stack_probe(depth - 1, current))
		return false;

	for (u32_t index = 0; index < 64; index++) {
		if (guard[index] != (0xa5000000 | index))
			return false;
	}

	return true;
}

void __kstart(unsigned long magic, unsigned long address)
{
	u32_t stack_pointer;
	bool stack_valid;

	(void)address;
	test_serial_init();
	test_serial_puts("TEST_START stack\n");

	if (magic != MULTIBOOT_BOOTLOADER_MAGIC) {
		test_serial_puts("TEST_FAIL invalid-multiboot-magic\n");
		test_exit(0x11);
	}

	__asm__ volatile("mov %%esp, %0" : "=r"(stack_pointer));
	stack_valid = (stack_pointer & 0x3) == 0 && stack_probe(8, 0);
	if (!stack_valid) {
		test_serial_puts("TEST_FAIL stack\n");
		test_exit(0x12);
	}

	test_serial_puts("TEST_PASS stack\n");
	test_serial_puts("TEST_RUN_COMPLETE PASS\n");
	test_exit(0x10);
}
