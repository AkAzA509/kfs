#include <unistd.h>
#include <stdio.h>

ssize_t kfs_write(int fd, const void *buf, size_t count)
{
	return write(fd, buf, count);
}

// int kfs_putchar(int c)
// {
// 	return putchar(c);
// }