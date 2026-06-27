#include "kprint.h"

static void	len_calc(int *total_len, int len)
{
	*total_len += len;
}

static int	flag_checker(const char *str, int i)
{
	int	flag = 0;

	if (str[i + 1] == '#') {
		flag = 1;
		i++;
	}
	if (str[i + 1] == ' ' ) {
		flag = 2;
		i += 2;
	}
	if (str[i + 1] == '+') {
		flag = 3;
		i++;
	}
	return flag;
}

static int	format_checker_fd(const char *str, va_list *ap, int i,
		int *total_len, int fd)
{
	int	flag = flag_checker(str, i);
	int	is_size = 0;

	if (flag > 0)
		i++;
	if (str[i + 1] == 'z') {
		is_size = 1;
		i++;
	}
	if (str[i + 1] == 'c')
		len_calc(total_len, ft_putchar(fd, va_arg(*ap, int)));
	if (str[i + 1] == 's')
		len_calc(total_len, s_format(fd, va_arg(*ap, char *)));
	if (str[i + 1] == 'p')
		len_calc(total_len, p_format(fd, va_arg(*ap, void *), 'p'));
	if (str[i + 1] == 'd' || str[i + 1] == 'i')
		len_calc(total_len, is_size ? zdi_format(fd, va_arg(*ap, ssize_t), flag) : di_format(fd, va_arg(*ap, int), flag));
	if (str[i + 1] == 'u')
		len_calc(total_len, is_size ? zu_format(fd, va_arg(*ap, size_t)) : u_format(fd, va_arg(*ap, unsigned int)));
	if (str[i + 1] == 'X' || str[i + 1] == 'x')
		len_calc(total_len, x_format(fd, va_arg(*ap, unsigned int), str[i + 1], flag));
	if (str[i + 1] == '%')
		len_calc(total_len, ft_putchar(fd, '%'));
	i++;
	return i;
}

static int	kprint_internal(int fd, const char *str, va_list *ap)
{
	int	i = 0;
	int	total_len = 0;

	while (str[i] != '\0')
	{
		if (str[i] == '%')
			i = format_checker_fd(str, ap, i, &total_len, fd);
		else {
			ft_putchar(fd, str[i]);
			len_calc(&total_len, 1);
		}
		i++;
	}
	return total_len;
}

int	kprint(const char *str, ...)
{
	int		total_len;
	va_list	ap;

	va_start(ap, str);
	total_len = kprint_internal(1, str, &ap);
	va_end(ap);

	return total_len;
}