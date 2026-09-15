#ifndef VFS_H
#define VFS_H

#include <stddef.h>

#define MAX_FD 32

#define O_RDONLY 0x01
#define O_WRONLY 0x02
#define O_RDWR (O_RDONLY | O_WRONLY)

#define EBADF 1

typedef enum {
	FD_TYPE_NONE = 0,
	FD_TYPE_CONSOLE, // Écran VGA / TTY
	FD_TYPE_SERIAL, // Port COM1 (UART) (klog)
	// FD_TYPE_PIPE,
	// FD_TYPE_FILE
} fd_type_t;

typedef struct {
	fd_type_t type;
	int flags; // O_*
	size_t offset;
	void *priv_data; // inode struct later
} kfile_t;

extern kfile_t kfile_table[MAX_FD];

void init_vfs(void);

#endif // VFS_H
