#include "test_support.h"
#include <fs/vfs.h>
#include <syscall/syscall.h>

static int screen_calls;
static int serial_calls;

int screen_puts(const char *value, size_t count)
{
	(void)value;
	screen_calls += (int)count;
	return (int)count;
}

int screen_fputs(const char *value, int screen, size_t count)
{
	(void)value;
	(void)screen;
	screen_calls += (int)count;
	return (int)count;
}

void serial_print(char value)
{
	(void)value;
	serial_calls++;
}

int klog(const char *format, ...)
{
	(void)format;
	return 0;
}

/* Validate write descriptor checks and routing to console and serial backends. */
void __kstart(unsigned long magic, unsigned long address)
{
	const char message[] = "ok";

	(void)address;
	test_serial_init();
	test_serial_puts("TEST_START sys-write\n");

	if (magic != MULTIBOOT_BOOTLOADER_MAGIC) {
		test_serial_puts("TEST_FAIL invalid-multiboot-magic\n");
		test_exit(0x11);
	}

	init_vfs();
	if (sys_write(-1, message, 2) != -EBADF ||
	    sys_write(0, message, 2) != -EBADF ||
	    sys_write(MAX_FD, message, 2) != -EBADF ||
	    sys_write(1, message, 2) != 2 || screen_calls != 2 ||
	    sys_write(2, message, 2) != 2 || serial_calls != 2) {
		test_serial_puts("TEST_FAIL sys-write-routing\n");
		test_exit(0x12);
	}

	test_serial_puts("TEST_PASS sys-write\n");
	test_serial_puts("TEST_RUN_COMPLETE PASS\n");
	test_exit(0x10);
}
