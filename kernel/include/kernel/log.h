#ifndef LOG_H
#define LOG_H

#include <stddef.h>
#include <stdint.h>

// If len = 0 the stack will be print entirely
// else all the value from start to len
void	log_stack(size_t len);

// klog() is a printf function who log on the serial port
int	klog(const char *restrict fmt, ...)
	__attribute__((format(printf, 1, 2)));

// serial_print_hex() write on the serial port in hexadecimal format
void serial_print_hex(u32_t val);

// serial_print() write normally on the serial port
void serial_print(char val);

#endif // LOG_H