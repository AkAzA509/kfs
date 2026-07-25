#ifndef LOG_H
#define LOG_H

#include <stddef.h>
#include <stdint.h>

// If len = 0 the stack will be print entirely
// else all the value from start to len
void	log_stack(size_t len);

// klog() is a printf like function who log on the serial port
int	klog(const char *restrict fmt, ...);

void serial_print_hex(u32_t val);
void serial_print(char val);


#endif // LOG_H