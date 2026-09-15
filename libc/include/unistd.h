#ifndef UNISTD_H
#define UNISTD_H

#include <stddef.h>

// proto of the syscall
extern ssize_t sys_write(int fd, const void *buf, size_t count);

ssize_t write(int fd, const void *buf, size_t size);

#endif // UNISTD_H