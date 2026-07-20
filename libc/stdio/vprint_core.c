#include "vprint_core.h"
#include "stdarg.h"
#include <stdbool.h>

typedef struct {
	// flags
	bool	minus, zero, plus, space, hash;

	// width / precision
	int		width;			// -1 if absent
	int		precision;		// -1 if absent (distinct from 0 !)

	// length modifier
	enum {
			LEN_NONE,
			LEN_HH,
			LEN_H,
			LEN_L,
			LEN_LL,
			LEN_Z,
			LEN_CAP_L
	}		length;

	// conversion
	char	conv;
}			format_spec_t;

static const char
*parse_flags(const char *restrict fmt, format_spec_t *spec)
{
	bool scanning = true;

	while (scanning) {
		switch (*fmt) {
			case '-': spec->minus = true; fmt++; break;
			case ' ': spec->space = true; fmt++; break;
			case '0': spec->zero = true; fmt++; break;
			case '+': spec->plus = true; fmt++; break;
			case '#': spec->hash = true; fmt++; break;
			default: scanning = false; break;
		}
	}
	return fmt;
}

static const char
*parse_width(const char *restrict fmt, format_spec_t *spec, va_list *ap)
{
	if (*fmt == '*') {
		spec->width = va_arg(*ap, int);
		fmt++;
	}
	else if (*fmt >='1' && *fmt <= '9') {
		int w = 0;
		while (*fmt >= '0' && *fmt <= '9') {
			w = w * 10 + (*fmt - '0');
			fmt++;
		}
		spec->width = w;
	}

	return fmt;
}

static const char
*parse_precision(const char *restrict fmt, format_spec_t *spec, va_list *ap)
{
	if (*fmt != '.')
		return fmt;

	fmt++;

	if (*fmt == '*') {
		spec->width = va_arg(*ap, int);
		fmt++;
	}
	else {
		int p = 0;
		while (*fmt >= '0' && *fmt <= '9') {
			p = p * 10 + (*fmt - '0');
			fmt++;
		}
		spec->precision = p;
	}

	return fmt;
}

static const char
*parse_length(const char *restrict fmt, format_spec_t *spec)
{
	if (*fmt == 'h') {
		if (*(fmt + 1) == 'h') {
			spec->length = LEN_HH;
			fmt += 2;
		}
		else {
			spec->length = LEN_H;
			fmt += 1;
		}
	}
	else if (*fmt == 'l') {
		if (*(fmt + 1) == 'l') {
			spec->length = LEN_LL;
			fmt += 2;
		}
		else {
			spec->length = LEN_L;
			fmt += 1;
		}
	}
	else if (*fmt == 'z') {
		spec->length = LEN_Z;
		fmt += 1;
	}
	else if (*fmt == 'L') {
		spec->length = LEN_CAP_L;
		fmt += 1;
	}

	return fmt;
}

static void
execute_conversion(format_spec_t *spec, va_list *ap, out_target_t *target)
{
	(void)spec;
	(void)ap;
	(void)target;
}

static const char
*parse_directive(const char *restrict fmt, va_list *ap, out_target_t *target)
{
	format_spec_t spec = {
		.width = -1,
		.precision = -1,
		.length = LEN_NONE
	};

	fmt = parse_flags(fmt, &spec);
	fmt = parse_width(fmt, &spec, ap);
	fmt = parse_precision(fmt, &spec, ap);
	fmt = parse_length(fmt, &spec);

	spec.conv = *fmt;
	fmt++;

	execute_conversion(&spec, ap, target);

	return fmt;
}

static void	out_char(out_target_t *target, char c)
{
	target->emit(target->ctx, c);
	target->count++;
}

int	vprint_core(const char *restrict fmt, va_list *ap, out_target_t *target)
{
	while (*fmt) {
		if (*fmt != '%') {
			out_char(target, *fmt);
			fmt++;
			continue;
		}
		fmt++;
		if (*fmt == '%') {
			out_char(target, '%');
			fmt++;
			continue;
		}
		fmt = parse_directive(fmt, ap, target);
	}
	return (int)target->count;
}
