#ifndef VFS_H
#define VFS_H

#include <stddef.h>
#include <stdint.h>

#define MAX_FD 32

#define O_RDONLY 0x01
#define O_WRONLY 0x02
#define O_RDWR (O_RDONLY | O_WRONLY)

#define IS_O_RDONLY(x) ((x) & O_RDONLY)
#define IS_O_WRONLY(x) ((x) & O_WRONLY)
#define IS_O_RDWR(x) (((x) & O_RDWR) == O_RDWR)

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
void add_fd_entry(u8_t idx, fd_type_t type, int flags);

#endif // VFS_H
