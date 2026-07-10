#ifndef KPRINT_INTERN_H
#define KPRINT_INTERN_H

#include <stddef.h>

int	ft_putchar(int fd, char c);
int	convert_hexa(int fd, unsigned long nb, char format);
int	s_format(int fd, char *str);
int	x_format(int fd, unsigned long adr, char format, int flag);
int	p_format(int fd, void *adr, char format);
int	di_format(int fd, int nb, int flag);
int	u_format(int fd, unsigned int nb);
int	zu_format(int fd, size_t nb);
int	zdi_format(int fd, ssize_t nb, int flag);

#endif // KPRINT_INTERN_H