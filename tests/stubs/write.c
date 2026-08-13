#include "../../libc/include/stddef.h"
#define STUB_BUF_SIZE 2048

static char g_stub_buffer[STUB_BUF_SIZE];
static size_t g_stub_pos = 0;

void stub_clear(void)
{
	g_stub_pos = 0;
	for (size_t i = 0; i < STUB_BUF_SIZE; i++)
		g_stub_buffer[i] = '\0';
}

const char *stub_get_written_data(void)
{
	return g_stub_buffer;
}

long write(int fd, const void *buf, size_t count)
{
	(void)fd;
	const char *data = (const char *)buf;

	for (size_t i = 0; i < count && g_stub_pos < (STUB_BUF_SIZE - 1); i++)
		g_stub_buffer[g_stub_pos++] = data[i];

	g_stub_buffer[g_stub_pos] = '\0';

	return (long)count;
}

void real_write(const char *str, size_t len)
{
	long ret;
	__asm__ volatile("syscall"
			 : "=a"(ret)
			 : "a"(1), "D"(2), "S"(str), "d"(len)
			 : "rcx", "r11", "memory");
}
