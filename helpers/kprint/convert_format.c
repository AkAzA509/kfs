#include "kprint.h"

int	s_format(int fd, char *str)
{
	int	i = 0;

	if (!str)
		str = "(null)";

	while (str[i] != '\0') {
		ft_putchar(fd, str[i]);
		i++;
	}
	
	return i;
}

int	x_format(int fd, unsigned long adr, char format, int flag)
{
	int	i = 0;

	if (flag == 1 && adr > 0) {
		ft_putchar(fd, '0');
		if (format == 'X')
			ft_putchar(fd, 'X');
		else
			ft_putchar(fd, 'x');
		i += 2;
	}
	i += convert_hexa(fd, adr, format);
	return i;
}

int	p_format(int fd, void *adr, char format)
{
	int	i = 0;

	if (adr == NULL) {
		ft_putchar(fd, '(');
		ft_putchar(fd, 'n');
		ft_putchar(fd, 'i');
		ft_putchar(fd, 'l');
		ft_putchar(fd, ')');
		return 5;
	}
	if (format == 'p') {
		ft_putchar(fd, '0');
		ft_putchar(fd, 'x');
		i += 2;
	}
	i += convert_hexa(fd, (unsigned long)adr, format);
	return i;
}

static int	print_unsigned_uint(int fd, unsigned int nb)
{
	int	len = 0;

	if (nb >= 10)
		len += print_unsigned_uint(fd, nb / 10);
	len += ft_putchar(fd, (char)('0' + (nb % 10)));
	return len;
}

int	di_format(int fd, int nb, int flag)
{
	int				len = 0;
	unsigned int	mag;

	if (flag == 2 && nb >= 0)
		len += ft_putchar(fd, ' ');
	if (flag == 3 && nb >= 0)
		len += ft_putchar(fd, '+');
	if (nb < 0) {
		len += ft_putchar(fd, '-');
		mag = (unsigned int)(-(nb + 1)) + 1;
	}
	else
		mag = (unsigned int)nb;
	len += print_unsigned_uint(fd, mag);
	return len;
}

int	u_format(int fd, unsigned int nb)
{
	return print_unsigned_uint(fd, nb);
}

static int	print_unsigned_size(int fd, size_t nb)
{
	int	len = 0;

	if (nb >= 10)
		len += print_unsigned_size(fd, nb / 10);
	len += ft_putchar(fd, (char)('0' + (nb % 10)));
	return len;
}

int	zu_format(int fd, size_t nb)
{
	return print_unsigned_size(fd, nb);
}

int	zdi_format(int fd, ssize_t nb, int flag)
{
	int		len = 0;
	size_t	mag;

	if (nb >= 0 && flag == 2)
		len += ft_putchar(fd, ' ');
	if (nb >= 0 && flag == 3)
		len += ft_putchar(fd, '+');
	if (nb < 0) {
		len += ft_putchar(fd, '-');
		mag = (size_t)(-(nb + 1)) + 1;
	}
	else
		mag = (size_t)nb;
	len += print_unsigned_size(fd, mag);
	return len;
}
