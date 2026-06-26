#ifndef KPRINT_H
#define KPRINT_H

#include "../../includes/stddef.h"
#include "../../includes/stdarg.h"

int	kprint(const char *format, ...);
int	ft_putchar(int fd, char c);
int	s_format(int fd, char *str);
int	x_format(int fd, unsigned long adr, char format, int flag);
int	p_format(int fd, void *adr, char format);
int	convert_hexa(int fd, unsigned long nb, char format);
int	di_format(int fd, int nb, int flag);
int	u_format(int fd, unsigned int nb);
int	zu_format(int fd, size_t nb);
int	zdi_format(int fd, ssize_t nb, int flag);

void	terminal_putchar(char c);

#endif // KPRINT_H
