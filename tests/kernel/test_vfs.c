#include "test_support.h"
#include <drivers/console.h>
#include <fs/vfs.h>

int klog(const char *format, ...)
{
	(void)format;
	return 0;
}

/* Validate the initial kernel file descriptor table. */
void __kstart(unsigned long magic, unsigned long address)
{
	(void)address;
	test_serial_init();
	test_serial_puts("TEST_START vfs\n");

	if (magic != MULTIBOOT_BOOTLOADER_MAGIC) {
		test_serial_puts("TEST_FAIL invalid-multiboot-magic\n");
		test_exit(0x11);
	}

	init_vfs();
	if (kfile_table[0].type != FD_TYPE_CONSOLE ||
	    kfile_table[0].flags != O_RDONLY ||
	    kfile_table[1].type != FD_TYPE_CONSOLE ||
	    kfile_table[1].flags != O_WRONLY ||
	    kfile_table[2].type != FD_TYPE_SERIAL ||
	    kfile_table[2].flags != O_WRONLY) {
		test_serial_puts("TEST_FAIL vfs-standard-fds\n");
		test_exit(0x12);
	}

	for (u8_t fd = 3; fd < MAX_SCREENS + 3; fd++) {
		if (kfile_table[fd].type != FD_TYPE_CONSOLE ||
		    kfile_table[fd].flags != O_WRONLY) {
			test_serial_puts("TEST_FAIL vfs-screen-fds\n");
			test_exit(0x13);
		}
	}

	add_fd_entry(MAX_FD, FD_TYPE_SERIAL, O_WRONLY);
	test_serial_puts("TEST_PASS vfs\n");
	test_serial_puts("TEST_RUN_COMPLETE PASS\n");
	test_exit(0x10);
}
