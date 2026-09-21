#include <drivers/console.h>
#include <syscall/syscall.h>
#include <core/log.h>
#include <fs/vfs.h>

#include <stddef.h>

static int console_write(kfile_t *file, int fd, const void *buf, size_t count)
{
	const char *data = (const char *)buf;

	if (!IS_O_WRONLY(file->flags))
		return -EBADF;

	if (fd == 1)
		screen_puts(data, count);
	else if (fd > 2 && fd < MAX_SCREENS)
		screen_fputs(data, fd - 3, count);
	else
		return -EBADF;

	return (int)count;
}

static int serial_write(kfile_t *file, const void *buf, size_t count)
{
	if (!IS_O_WRONLY(file->flags))
		return -EBADF;

	const char *data = (const char *)buf;
	for (size_t i = 0; i < count; i++)
		serial_print(data[i]);
	return (int)count;
}

ssize_t sys_write(int fd, const void *buf, size_t count)
{
	if (fd <= 0 || fd >= MAX_FD || kfile_table[fd].type == FD_TYPE_NONE)
		return -EBADF;

	kfile_t *file = &kfile_table[fd];
	if (!file)
		return -EBADF;

	switch (file->type) {
	case FD_TYPE_CONSOLE:
		return console_write(file, fd, buf, count);
	case FD_TYPE_SERIAL:
		return serial_write(file, buf, count);
	default:
		return -EBADF;
	}
}
