#include "vprint_core.h"
#include "stdarg.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

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
			LEN_Z
	}		length;

	// conversion
	char	conv;
}			format_spec_t;

// ----- Args parser ----- //

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
		spec->precision = va_arg(*ap, int);
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
	return fmt;
}

// ----- Lenght getter ----- //

static long long	get_int_arg(format_spec_t *spec, va_list *ap)
{
	switch (spec->length) {
		case LEN_HH: return (signed char)va_arg(*ap, int);
		case LEN_H: return (short)va_arg(*ap, int);
		case LEN_L: return va_arg(*ap, long);
		case LEN_LL: return va_arg(*ap, long long);
		case LEN_Z: return va_arg(*ap, size_t);
		default: return va_arg(*ap, int);
	}
}

static unsigned long long	get_uint_arg(format_spec_t *spec, va_list *ap)
{
	switch (spec->length) {
		case LEN_HH: return (unsigned char)va_arg(*ap, int);
		case LEN_H: return (unsigned short)va_arg(*ap, int);
		case LEN_L: return va_arg(*ap, unsigned long);
		case LEN_LL: return va_arg(*ap, unsigned long long);
		case LEN_Z: return va_arg(*ap, size_t);
		default: return va_arg(*ap, unsigned int);
	}
}

// ----- Convertion core ----- //

static void	out_char(out_target_t *target, char c)
{
	target->emit(target->ctx, c);
	target->count++;
}

static int
get_prefix(char *prefix_buf, format_spec_t *spec,
	unsigned long long val, int base, bool uppercase)
{
	if (!spec->hash || val == 0)
		return 0;

	if (base == 16) {
		prefix_buf[0] = '0';
		prefix_buf[1] = uppercase ? 'X' : 'x';
		return 2;
	}
	if (base == 8) {
		prefix_buf[0] = '0';
		return 1;
	}
	return 0;
}

static void
get_sign_and_abs(long long val, char *sign, unsigned long long *abs_val, format_spec_t *spec)
{
	if (val < 0) {
		*sign = '-';
		*abs_val = -(unsigned long long)val; // to protect the var overflow if val is LLONG_MIN
	}
	else {
		*abs_val = (unsigned long long)val;
		if (spec->plus)
			*sign = '+';
		else if (spec->space)
			*sign = ' ';
		else
			*sign = '\0';
	}
}

static char
*apply_precision(char *digits, int digits_len, char *buf_start,
	format_spec_t *spec, unsigned long long val, int *out_len)
{
	if (spec->precision == 0 && val == 0) {
		*out_len = 0;
		return digits;
	}

	int pad = spec->precision - digits_len;

	while (pad > 0 && digits > buf_start) {
		*(--digits) = '0';
		pad--;
	}

	*out_len = (spec->precision > digits_len) ? spec->precision : digits_len;
	return digits;
}

static void
emit_prefixed_number(out_target_t *target, char *prefix, int prefix_len,
				char *digits, int len, format_spec_t *spec, int pad_len)
{
	bool	zero_pad = spec->zero && !spec->minus && spec->precision == -1;


	if (!spec->minus && !zero_pad) {
		while (pad_len-- > 0)
			out_char(target, ' ');
	}

	for (int i = 0; i < prefix_len; i++)
		out_char(target, prefix[i]);

	if (zero_pad)
		while (pad_len-- > 0)
			out_char(target, '0');

	for (int i = 0; i < len; i++)
		out_char(target, digits[i]);

	if (spec->minus)
		while (pad_len-- > 0)
			out_char(target, ' ');
}

static char
*uint_to_str(unsigned long long val, char *buf_end, int base, bool uppercase)
{
	const char	*digits = uppercase ? "0123456789ABCDEF" : "0123456789abcdef";
	char	*p = buf_end;

	*p = '\0';
	if (val == 0)
		*(--p) = '0';
	else {
		while (val > 0) {
			*(--p) = digits[val % base];
			val /= base;
		}
	}
	return p;
}

static void
conv_signed(format_spec_t *spec, va_list *ap, out_target_t *target)
{
	long long	val = get_int_arg(spec, ap);

	char	sign;
	unsigned long long	abs_val;
	get_sign_and_abs(val, &sign, &abs_val, spec);

	char	tmp[32];
	char	*digits = uint_to_str(abs_val, tmp + sizeof(tmp) - 1, 10, false);
	int		digits_len = tmp + sizeof(tmp) - 1 - digits;

	int	len;
	digits = apply_precision(digits, digits_len, tmp, spec, abs_val, &len);

	char	prefix[1];
	int		prefix_len = 0;
	if (sign != '\0') {
		prefix[0] = sign;
		prefix_len = 1;
	}

	int	total_len = prefix_len + len;
	int	pad_len = (spec->width > total_len) ? spec->width - total_len : 0;

	emit_prefixed_number(target, prefix, prefix_len, digits, len, spec, pad_len);
}

static void
conv_unsigned(format_spec_t *spec, va_list *ap,
		out_target_t *target, int base, bool uppercase)
{
	unsigned long long	val = get_uint_arg(spec, ap);

	char	tmp[32];
	char	*digits = uint_to_str(val, tmp + sizeof(tmp) - 1, base, uppercase);
	int		digits_len = tmp + sizeof(tmp) - 1 - digits;

	int	len;
	digits = apply_precision(digits, digits_len, tmp, spec, val, &len);

	char	prefix[2];
	int	prefix_len = get_prefix(prefix, spec, val, base, uppercase);

	int	total_len = prefix_len + len;
	int	pad_len = (spec->width > total_len) ? spec->width - total_len : 0;

	emit_prefixed_number(target, prefix, prefix_len, digits, len, spec, pad_len);
}

static void conv_string(format_spec_t *spec, va_list *ap, out_target_t *target)
{
	const char	*str = va_arg(*ap, const char *);
	if (str == NULL)
		str = "(null)";

	int	len = (int)strlen(str);

	if (spec->precision != -1 && spec->precision < len)
		len = spec->precision;

	int	pad_len = (spec->width > len) ? spec->width - len : 0;

	if (!spec->minus)
		while (pad_len-- > 0)
			out_char(target, ' ');

	for (int i = 0; i < len; i++)
		out_char(target, str[i]);

	if (spec->minus)
		while (pad_len-- > 0)
			out_char(target, ' ');
}

static void conv_char(format_spec_t *spec, va_list *ap, out_target_t *target)
{
	char	c = (char)va_arg(*ap, int);

	int	pad_len = (spec->width > 1) ? spec->width - 1 : 0;

	if (!spec->minus)
		while (pad_len-- > 0)
			out_char(target, ' ');

	out_char(target, c);

	if (spec->minus)
		while (pad_len-- > 0)
			out_char(target, ' ');
}

static void conv_pointer(format_spec_t *spec, va_list *ap, out_target_t *target)
{
	void	*ptr = va_arg(*ap, void *);

	if (ptr == NULL) {
		const	char *nil_str = "(nil)";
		int	len = 5;
		int	pad_len = (spec->width > len) ? spec->width - len : 0;

		if (!spec->minus)
			while (pad_len-- > 0)
				out_char(target, ' ');
		for (int i = 0; i < len; i++)
			out_char(target, nil_str[i]);
		if (spec->minus)
			while (pad_len-- > 0)
				out_char(target, ' ');
		return;
	}

	uintptr_t val = (uintptr_t)ptr;

	char	tmp[32];
	char	*digits = uint_to_str(val, tmp + sizeof(tmp) - 1, 16, false);
	int		digits_len = tmp + sizeof(tmp) - 1 - digits;

	char	prefix[2] = {'0', 'x'};
	int		total_len = 2 + digits_len;
	int		pad_len = (spec->width > total_len) ? spec->width - total_len : 0;

	format_spec_t	p_spec = *spec;
	p_spec.precision = -1;

	emit_prefixed_number(target, prefix, 2, digits, digits_len, &p_spec, pad_len);
}

// ----- Floating point handling ----- //

// [ 1 bit signe ][ 11 bits exposant ][ 52 bits mantisse (fraction) ]
// bit 63         bits 62-52           bits 51-0
// valeur = (-1)^signe × 1.mantisse × 2^(exposant_stocké - 1023)

typedef union {
	double d;
	u64_t bits;
} double_bits_t;

typedef struct {
	u64_t significand;
	int shift;
} float_decomp_t;

static float_decomp_t decompose_significand(int exponent, u64_t mantissa_bits, int exp_stored)
{
	float_decomp_t r;
	if (exp_stored == 0) {
		r.significand = mantissa_bits;
		r.shift = -1022 - 52;
	} else {
		r.significand = (1ULL << 52) | mantissa_bits;
		r.shift = exponent - 52;
	}
	return r;
}

static void split_int_frac(float_decomp_t d, u64_t *int_part, u64_t *frac_num, int *n)
{
	if (d.shift >= 0) {
		*int_part = (d.shift < 64) ? (d.significand << d.shift) : 0;
		*frac_num = 0;
		*n = 0;
	} else {
		*n = (-d.shift < 64) ? -d.shift : 63;
		*int_part = d.significand >> *n;
		*frac_num = d.significand & ((1ULL << *n) - 1);
	}
}

static int extract_frac_digits_rounded(u64_t frac_num, int n, int precision, char *out, u64_t *int_part)
{
	char digits[32];
	for (int i = 0; i < precision + 1; i++) {
		frac_num *= 10;
		digits[i] = (char)(n > 0 ? (frac_num >> n) : 0);
		if (n > 0)
			frac_num &= (1ULL << n) - 1;
	}

	bool round_up = digits[precision] >= 5;
	int i = precision - 1;
	while (round_up && i >= 0) {
		digits[i]++;
		if (digits[i] == 10) { digits[i] = 0; i--; }
		else round_up = false;
	}
	if (round_up)
		(*int_part)++;

	for (int j = 0; j < precision; j++)
		out[j] = '0' + digits[j];
	return precision;
}

static void emit_special_float(out_target_t *target, format_spec_t *spec, const char *str)
{
	int len = 0;
	while (str[len]) len++;
	int pad_len = (spec->width > len) ? spec->width - len : 0;
	if (!spec->minus)
		while (pad_len-- > 0) out_char(target, ' ');
	for (int i = 0; i < len; i++)
		out_char(target, str[i]);
	if (spec->minus)
		while (pad_len-- > 0) out_char(target, ' ');
}

static void conv_float(format_spec_t *spec, va_list *ap, out_target_t *target)
{
	double	val = va_arg(*ap, double);
	double_bits_t	u;
	u.d = val;

	int		sign_bit = (u.bits >> 63) & 0x1;
	int		exp_stored = (u.bits >> 52) & 0x7FF;
	u64_t	mantissa_bits = u.bits & 0xFFFFFFFFFFFFF;
	int		exponent = exp_stored - 1023;

	if (exp_stored == 0x7FF) {
		emit_special_float(target, spec, (mantissa_bits != 0) ? "nan" : (sign_bit ? "-inf" : "inf"));
		return;
	}

	int	precision = (spec->precision == -1) ? 6 : spec->precision;
	if (precision > 30)
		precision = 30;

	float_decomp_t	decomp = decompose_significand(exponent, mantissa_bits, exp_stored);

	u64_t	int_part, frac_num;
	int		n;
	split_int_frac(decomp, &int_part, &frac_num, &n);

	char	frac_digits[32];
	extract_frac_digits_rounded(frac_num, n, precision, frac_digits, &int_part);

	char	tmp[32];
	char	*int_digits = uint_to_str(int_part, tmp + sizeof(tmp) - 1, 10, false);
	int		int_len = tmp + sizeof(tmp) - 1 - int_digits;

	char	sign = '\0';
	if (sign_bit)
		sign = '-';
	else if (spec->plus)
		sign = '+';
	else if (spec->space)
		sign = ' ';

	int		total_len = (sign != '\0' ? 1 : 0) + int_len + (precision > 0 ? 1 + precision : 0);
	int		pad_len = (spec->width > total_len) ? spec->width - total_len : 0;
	bool	zero_pad = spec->zero && !spec->minus;

	if (!spec->minus && !zero_pad)
		while (pad_len-- > 0)
			out_char(target, ' ');
	if (sign != '\0')
		out_char(target, sign);
	if (zero_pad)
		while (pad_len-- > 0)
			out_char(target, '0');
	for (int i = 0; i < int_len; i++)
		out_char(target, int_digits[i]);
	if (precision > 0) {
		out_char(target, '.');
		for (int i = 0; i < precision; i++)
			out_char(target, frac_digits[i]);
	}
	if (spec->minus)
		while (pad_len-- > 0)
			out_char(target, ' ');
}

static void
execute_conversion(format_spec_t *spec, va_list *ap, out_target_t *target)
{
	switch (spec->conv) {
		case 'd':
		case 'i':
			conv_signed(spec, ap, target);
			break;
		case 'u':
			conv_unsigned(spec, ap, target, 10, false);
			break;
		case 'x':
			conv_unsigned(spec, ap, target, 16, false);
			break;
		case 'X':
			conv_unsigned(spec, ap, target, 16, true);
			break;
		case 'o':
			conv_unsigned(spec, ap, target, 8, false);
			break;
		case 's':
			conv_string(spec, ap, target);
			break;
		case 'c':
			conv_char(spec, ap, target);
			break;
		case 'p':
			conv_pointer(spec, ap, target);
			break;
		case 'f':
			conv_float(spec, ap, target);
			break;
		case 'F':
			conv_float(spec, ap, target);
			break;
		default:
			// conversion inconnue : comportement défensif
			out_char(target, '%');
			if (spec->conv)
				out_char(target, spec->conv);
			break;
	}
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
