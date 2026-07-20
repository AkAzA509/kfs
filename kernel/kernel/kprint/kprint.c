#include "kernel/kprint/kprint.h"
#include "kprint_intern.h"
#include <arch/i386/tty.h>
#include <cassert>
#include <stdarg.h>
#include <stdbool.h>

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

// static int	kprint_internal(int fd, const char *str, va_list *ap)
// {
// 	int	i = 0;
// 	int	total_len = 0;

// 	while (str[i] != '\0')
// 	{
// 		if (str[i] == '%')
// 			i = format_checker_fd(str, ap, i, &total_len, fd);
// 		else {
// 			ft_putchar(fd, str[i]);
// 			len_calc(&total_len, 1);
// 		}
// 		i++;
// 	}
// 	return total_len;
// }

// int	kprint(const char *str, ...)
// {
// 	int		total_len;
// 	va_list	ap;

// 	va_start(ap, str);
// 	total_len = kprint_internal(1, str, &ap);
// 	va_end(ap);

// 	return total_len;
// }

static int	putchar_fd(const char c, int fd)
{
	(void)fd;
	putchar(c);
	return 1;
}

int	ptr_format(int fd, unsigned long addr, char type)
{
	int wrt = 0;
	int flag = 1; // to change when a flag handler whas do

	if (!addr) {
		kwrite("(nil)", 5);
		return 5;
	}
		
	if (type == 'p' || (flag && (type == 'x' || type == 'X'))) {
		if (type == 'X')
			kwrite("0X", 2);
		else
			kwrite("0x", 2);
		wrt += 2;
	}

	wrt += convert_hexa(fd, addr, type);
	return wrt;
}

int	z_format(int fd, va_list *ap, const char *restrict fmt)
{
	if (*fmt == 'z') {
		fmt++;
		if (*fmt + 1 == 'd' || *fmt + 1 == 'i')
			return zdi_format(fd, va_arg(*ap, ssize_t), 0); // placeholder at 0 to change for flag
		else if (*fmt + 1 == 'u')
			return zu_format(fd, va_arg(*ap, size_t));
		else
			; // handle unrecognize fmt specifier
	}

	if (*fmt == 'd' || *fmt == 'i')
		return di_format(fd, va_arg(*ap, int), 0); // placeholder at 0 to change for flag
	else if (*fmt == 'u')
		return u_format(fd, va_arg(*ap, unsigned int));
}

static void	convert(int fd, const char *restrict fmt, va_list *ap, int *wrt_len)
{
	bool	is_flag = check_flag(fmt);
	bool	is_lenght_mod = check_len_mod(fmt);

	if (*fmt == 'c' || *fmt == 's')
		*wrt_len += s_format(fd, va_arg(*ap, char *));
	else if (*fmt == 'p' || *fmt == 'x' || *fmt == 'X')
		*wrt_len += ptr_format(fd, va_arg(*ap, unsigned int), *fmt);
	else if (*fmt == 'z' || *fmt == 'd' || *fmt == 'i')
		*wrt_len += z_format(fd, ap, fmt);
	else if (*fmt == '%')
		*wrt_len += putchar_fd('%', fd);
	fmt++;
}

static void	kprint_internal(int fd, const char *restrict fmt, va_list *ap, int *wrt_len)
{
	while(fmt) {
		if (*fmt == '%')
			convert(fd, ++fmt, ap, wrt_len);
		else
			*wrt_len += putchar_fd(*fmt, fd);
		fmt++;
	}
}

int kprint(const char *restrict fmt, ...)
{
	int	wrt_len = 0;
	va_list	ap;

	va_start(ap, fmt);
	kprint_internal(1, fmt, &ap, &wrt_len);
	va_end(ap);
	
	return wrt_len;
}