#ifndef TEST_SUPPORT_H
#define TEST_SUPPORT_H

#include <stdint.h>

#define MULTIBOOT_BOOTLOADER_MAGIC 0x2BADB002

void test_serial_init(void);
void test_serial_puts(const char *value);
__attribute__((noreturn)) void test_exit(u8_t code);

#endif // TEST_SUPPORT_H
