#include <arch/i386/console.h>
#include <stddef.h>

int	kputchar(char c)
{
	return screen_putchar(c);
}

ssize_t	kwrite(int fd, const void* data, size_t size)
{
	(void)fd;
	const char	*str = data;

	size_t i = 0;
	while(i < size) {
		kputchar(str[i]);
		i++;
	}
	return (ssize_t)i;
}

// The libc syscall wrapper around the kernel implementation
ssize_t write(int fd, const void *buf, size_t count)
{
	return kwrite(fd, buf, count);
}