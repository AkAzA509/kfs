#include <stddef.h>
#include <unistd.h>

ssize_t write(int fd, const void *buf, size_t size)
{
	return sys_write(fd, buf, size);
}