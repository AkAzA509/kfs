#ifndef SYSCALL_H
#define SYSCALL_H

#include <stddef.h>

ssize_t sys_write(int fd, const void *buf, size_t count);

#endif // SYSCALL_H