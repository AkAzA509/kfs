#include <drivers/console.h>
#include <kernel/log.h>
#include <fs/vfs.h>
#include <stddef.h>

static int console_write(kfile_t *file, int fd, const void *buf, size_t count)
{
	(void)file;
	const char *data = (const char *)buf;

	switch (fd) {
	case 0:
		return -EBADF;
	case 1:
	case 3:
		// vterm_write(&vterm[0], data, count);
		// break;
	case 4:
		// vterm_write(&vterm[1], data, count);
		// break;
	case 5:
		// vterm_write(&vterm[2], data, count);
		// break;
	case 6:
		// vterm_write(&vterm[2], data, count);
		// break;
		for (size_t i = 0; i < count; i++)
			screen_putchar(data[i]);
		break;
	default:
		return -1;
	}
	return (int)count;
}

static int serial_write(kfile_t *file, int fd, const void *buf, size_t count)
{
	(void)file;

	if (fd != 2)
		return -EBADF;

	const char *data = (const char *)buf;
	for (size_t i = 0; i < count; i++)
		serial_print(data[i]);
	return (int)count;
}

ssize_t sys_write(int fd, const void *buf, size_t count)
{
	if (fd < 0 || fd >= MAX_FD || kfile_table[fd].type == FD_TYPE_NONE)
		return -EBADF;

	kfile_t *file = &kfile_table[fd];
	switch (file->type) {
	case FD_TYPE_CONSOLE:
		return console_write(file, fd, buf, count);
	case FD_TYPE_SERIAL:
		return serial_write(file, fd, buf, count);
	default:
		return -EBADF;
	}
}