#include <stdio.h>
#include <unistd.h>

int putchar(int c)
{
	unsigned char ch = (unsigned char)c;
	if (write(stdout->fd, &ch, 1) != 1)
		return -1;
	return c;
}