#include "../../kernel/terminal.h"

int	ft_putchar(int fd, char c)
{
	(void)fd;
	kputchar(c);
	return 1;
}

int	convert_hexa(int fd, unsigned long nb, char format)
{
	int	i = 0;

	if (nb >= 16) {
		i += convert_hexa(fd, nb / 16, format);
		i += convert_hexa(fd, nb % 16, format);
	}
	else {
		if (format == 'X')
			i += ft_putchar(fd, "0123456789ABCDEF"[nb % 16]);
		else
			i += ft_putchar(fd, "0123456789abcdef"[nb % 16]);
	}
	return i;
}
