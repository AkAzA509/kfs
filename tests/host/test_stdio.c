#include "test.h"

#include <limits.h>
#include <stdio.h>
#include <unistd.h>

static int write_fd;
static int write_result = 1;
static char write_buffer[BUFSIZE * 2];
static size_t write_length;

ssize_t sys_write(int fd, const void *buffer, size_t count)
{
	const char *source = buffer;

	write_fd = fd;
	if (write_result <= 0)
		return write_result;
	for (size_t i = 0; i < count && write_length < sizeof(write_buffer);
	     ++i)
		write_buffer[write_length++] = source[i];
	return (ssize_t)count;
}

static void reset_write_capture(void)
{
	write_fd = -1;
	write_result = 1;
	write_length = 0;
}

static int format_with_vsnprintf(char *buffer, size_t size, const char *format,
				 ...)
{
	va_list ap;

	va_start(ap, format);
	int result = vsnprintf(buffer, size, format, ap);
	va_end(ap);
	return result;
}

static int format_with_vsprintf(char *buffer, const char *format, ...)
{
	va_list ap;

	va_start(ap, format);
	int result = vsprintf(buffer, format, ap);
	va_end(ap);
	return result;
}

static int format_with_vprintf(const char *format, ...)
{
	va_list ap;

	va_start(ap, format);
	int result = vprintf(format, ap);
	va_end(ap);
	return result;
}

static int format_with_vfprintf(FILE *stream, const char *format, ...)
{
	va_list ap;

	va_start(ap, format);
	int result = vfprintf(stream, format, ap);
	va_end(ap);
	return result;
}

static int format_with_vdprintf(int fd, const char *format, ...)
{
	va_list ap;

	va_start(ap, format);
	int result = vdprintf(fd, format, ap);
	va_end(ap);
	return result;
}

static int format_dynamic(char *buffer, size_t size, const char *format, ...)
{
	va_list ap;

	va_start(ap, format);
	int result = vsnprintf(buffer, size, format, ap);
	va_end(ap);
	return result;
}

static void snprintf_formats_literal_text(void)
{
	char buffer[32];

	TEST_ASSERT_INT(11, snprintf(buffer, sizeof(buffer), "hello world"));
	TEST_ASSERT_STR("hello world", buffer);
	TEST_ASSERT_INT(2, snprintf(buffer, sizeof(buffer), "%%%c", '%'));
	TEST_ASSERT_STR("%%", buffer);
}

static void snprintf_formats_signed_integers(void)
{
	char buffer[64];

	TEST_ASSERT_INT(5, snprintf(buffer, sizeof(buffer), "%d %i", -42, 7));
	TEST_ASSERT_STR("-42 7", buffer);
	TEST_ASSERT_INT(3, snprintf(buffer, sizeof(buffer), "%+d", 42));
	TEST_ASSERT_STR("+42", buffer);
	TEST_ASSERT_INT(3, snprintf(buffer, sizeof(buffer), "% d", 42));
	TEST_ASSERT_STR(" 42", buffer);
}

static void snprintf_formats_unsigned_integers_in_each_base(void)
{
	char buffer[64];

	TEST_ASSERT_INT(11, snprintf(buffer, sizeof(buffer), "%u %x %X %o", 42U,
				     0x2aU, 0x2aU, 052U));
	TEST_ASSERT_STR("42 2a 2A 52", buffer);
	TEST_ASSERT_INT(8,
			snprintf(buffer, sizeof(buffer), "%#x %#o", 42U, 42U));
	TEST_ASSERT_STR("0x2a 052", buffer);
}

static void snprintf_handles_integer_lengths(void)
{
	char buffer[64];
	long long signed_value = -123456789012LL;
	unsigned long long unsigned_value = 0xfedcba9876543210ULL;

	TEST_ASSERT_INT(13,
			snprintf(buffer, sizeof(buffer), "%lld", signed_value));
	TEST_ASSERT_STR("-123456789012", buffer);
	TEST_ASSERT_INT(16, snprintf(buffer, sizeof(buffer), "%llx",
				     unsigned_value));
	TEST_ASSERT_STR("fedcba9876543210", buffer);
	TEST_ASSERT_INT(2, snprintf(buffer, sizeof(buffer), "%hhd", 255));
	TEST_ASSERT_STR("-1", buffer);
}

static void snprintf_applies_width_and_alignment(void)
{
	char buffer[64];

	TEST_ASSERT_INT(5, snprintf(buffer, sizeof(buffer), "%5d", 42));
	TEST_ASSERT_STR("   42", buffer);
	TEST_ASSERT_INT(5, snprintf(buffer, sizeof(buffer), "%-5d", 42));
	TEST_ASSERT_STR("42   ", buffer);
	TEST_ASSERT_INT(5, snprintf(buffer, sizeof(buffer), "%05d", 42));
	TEST_ASSERT_STR("00042", buffer);
	TEST_ASSERT_INT(5, snprintf(buffer, sizeof(buffer), "%*d", 5, 42));
	TEST_ASSERT_STR("   42", buffer);
	TEST_ASSERT_INT(5, snprintf(buffer, sizeof(buffer), "%*d", -5, 42));
	TEST_ASSERT_STR("42   ", buffer);
}

static void snprintf_applies_precision(void)
{
	char buffer[64];

	TEST_ASSERT_INT(5, snprintf(buffer, sizeof(buffer), "%.5d", 42));
	TEST_ASSERT_STR("00042", buffer);
	TEST_ASSERT_INT(5, snprintf(buffer, sizeof(buffer), "%.5u", 42U));
	TEST_ASSERT_STR("00042", buffer);
	TEST_ASSERT_INT(0, snprintf(buffer, sizeof(buffer), "%.0d", 0));
	TEST_ASSERT_STR("", buffer);
	TEST_ASSERT_INT(3, snprintf(buffer, sizeof(buffer), "%.3s", "kernel"));
	TEST_ASSERT_STR("ker", buffer);
}

static void snprintf_formats_strings_and_characters(void)
{
	char buffer[64];

	TEST_ASSERT_INT(8, snprintf(buffer, sizeof(buffer), "%s %c", "kernel",
				    '!'));
	TEST_ASSERT_STR("kernel !", buffer);
	TEST_ASSERT_INT(8, snprintf(buffer, sizeof(buffer), "%8s", "ok"));
	TEST_ASSERT_STR("      ok", buffer);
	TEST_ASSERT_INT(8, snprintf(buffer, sizeof(buffer), "%-8s", "ok"));
	TEST_ASSERT_STR("ok      ", buffer);
	const char *null_string = NULL;
	TEST_ASSERT_INT(6, snprintf(buffer, sizeof(buffer), "%s", null_string));
	TEST_ASSERT_STR("(null)", buffer);
}

static void snprintf_reports_required_size_when_truncated(void)
{
	char buffer[5];

	TEST_ASSERT_INT(10, snprintf(buffer, sizeof(buffer), "0123456789"));
	TEST_ASSERT_STR("0123", buffer);
	TEST_ASSERT_INT(3, snprintf(buffer, 1, "abc"));
	TEST_ASSERT_STR("", buffer);
	TEST_ASSERT_INT(3, snprintf(buffer, 0, "abc"));
	TEST_ASSERT_STR("", buffer);
}

static void sprintf_writes_an_unbounded_result(void)
{
	char buffer[64];

	TEST_ASSERT_INT(9, sprintf(buffer, "%s:%04d", "port", 7));
	TEST_ASSERT_STR("port:0007", buffer);
}

static void snprintf_supports_float_formats(void)
{
	char buffer[64];

	TEST_ASSERT_INT(5, snprintf(buffer, sizeof(buffer), "%.2f", 12.345));
	TEST_ASSERT_STR("12.35", buffer);
	TEST_ASSERT_INT(6, snprintf(buffer, sizeof(buffer), "%+.2f", 12.345));
	TEST_ASSERT_STR("+12.35", buffer);
	TEST_ASSERT_INT(2, snprintf(buffer, sizeof(buffer), "%.0f", 12.5));
	TEST_ASSERT_STR("13", buffer);
}

static void snprintf_supports_special_float_values(void)
{
	char buffer[32];

	TEST_ASSERT_INT(3, snprintf(buffer, sizeof(buffer), "%f", 1.0 / 0.0));
	TEST_ASSERT_STR("inf", buffer);
	TEST_ASSERT_INT(4, snprintf(buffer, sizeof(buffer), "%f", -1.0 / 0.0));
	TEST_ASSERT_STR("-inf", buffer);
}

static void snprintf_accepts_dynamic_precision(void)
{
	char buffer[32];

	TEST_ASSERT_INT(5, snprintf(buffer, sizeof(buffer), "%.*d", 5, 12));
	TEST_ASSERT_STR("00012", buffer);
}

static void snprintf_combines_sign_width_and_zero_padding(void)
{
	char buffer[32];

	TEST_ASSERT_INT(6, snprintf(buffer, sizeof(buffer), "%+06d", 42));
	TEST_ASSERT_STR("+00042", buffer);
	TEST_ASSERT_INT(6, snprintf(buffer, sizeof(buffer), "%-+6d", 42));
	TEST_ASSERT_STR("+42   ", buffer);
}

static void snprintf_handles_hash_zero_and_negative_precision(void)
{
	char buffer[32];

	TEST_ASSERT_INT(1, snprintf(buffer, sizeof(buffer), "%#x", 0U));
	TEST_ASSERT_STR("0", buffer);
	TEST_ASSERT_INT(2, snprintf(buffer, sizeof(buffer), "%.*d", -1, 42));
	TEST_ASSERT_STR("42", buffer);
}

static void snprintf_handles_long_and_size_arguments(void)
{
	char buffer[64];
	long signed_value = -12345L;
	size_t unsigned_value = 12345U;

	TEST_ASSERT_INT(6,
			snprintf(buffer, sizeof(buffer), "%ld", signed_value));
	TEST_ASSERT_STR("-12345", buffer);
	TEST_ASSERT_INT(5, snprintf(buffer, sizeof(buffer), "%zu",
				    unsigned_value));
	TEST_ASSERT_STR("12345", buffer);
}

static void snprintf_handles_unknown_conversions_as_literal_text(void)
{
	char buffer[32];
	char format[4];

	format[0] = 'x';
	format[1] = '%';
	format[2] = 'q';
	format[3] = '\0';

	TEST_ASSERT_INT(3, format_dynamic(buffer, sizeof(buffer), format));
	TEST_ASSERT_STR("x%q", buffer);
}

static void snprintf_applies_width_to_floats(void)
{
	char buffer[32];

	TEST_ASSERT_INT(8, snprintf(buffer, sizeof(buffer), "%08.2f", 1.5));
	TEST_ASSERT_STR("00001.50", buffer);
	TEST_ASSERT_INT(8, snprintf(buffer, sizeof(buffer), "%-8.2f", 1.5));
	TEST_ASSERT_STR("1.50    ", buffer);
}

static void snprintf_handles_integer_extremes(void)
{
	char buffer[64];

	TEST_ASSERT_INT(11, snprintf(buffer, sizeof(buffer), "%d", INT_MIN));
	TEST_ASSERT_STR("-2147483648", buffer);
	TEST_ASSERT_INT(20, snprintf(buffer, sizeof(buffer), "%lld",
				     (long long)LLONG_MIN));
	TEST_ASSERT_STR("-9223372036854775808", buffer);
	TEST_ASSERT_INT(20, snprintf(buffer, sizeof(buffer), "%llu",
				     (unsigned long long)ULLONG_MAX));
	TEST_ASSERT_STR("18446744073709551615", buffer);
}

static void snprintf_handles_multiple_dynamic_arguments(void)
{
	char buffer[64];

	TEST_ASSERT_INT(7, snprintf(buffer, sizeof(buffer), "%*.*d", 7, 4, 12));
	TEST_ASSERT_STR("   0012", buffer);
	TEST_ASSERT_INT(8, snprintf(buffer, sizeof(buffer), "%*.*s", -8, 3,
				    "kernel"));
	TEST_ASSERT_STR("ker     ", buffer);
}

static void snprintf_handles_empty_and_boundary_buffers(void)
{
	char buffer[2] = { 'x', 'y' };
	char empty_format[] = "";

	TEST_ASSERT_INT(0,
			format_dynamic(buffer, sizeof(buffer), empty_format));
	TEST_ASSERT_STR("", buffer);
	TEST_ASSERT_INT(1, snprintf(buffer, sizeof(buffer), "a"));
	TEST_ASSERT_STR("a", buffer);
	TEST_ASSERT_INT(1, snprintf(buffer, 1, "a"));
	TEST_ASSERT_STR("", buffer);
	TEST_ASSERT_INT(1, snprintf(NULL, 0, "a"));
}

static void snprintf_handles_float_rounding_boundaries(void)
{
	char buffer[64];

	TEST_ASSERT_INT(5, snprintf(buffer, sizeof(buffer), "%.2f", 9.999));
	TEST_ASSERT_STR("10.00", buffer);
	TEST_ASSERT_INT(4, snprintf(buffer, sizeof(buffer), "%.2f", 0.004));
	TEST_ASSERT_STR("0.00", buffer);
	TEST_ASSERT_INT(5, snprintf(buffer, sizeof(buffer), "%.2f", -0.004));
	TEST_ASSERT_STR("-0.00", buffer);
	TEST_ASSERT_INT(4, snprintf(buffer, sizeof(buffer), "%.2F", 1.5));
	TEST_ASSERT_STR("1.50", buffer);
}

static void snprintf_handles_float_sign_and_zero(void)
{
	char buffer[64];

	TEST_ASSERT_INT(2, snprintf(buffer, sizeof(buffer), "%+.0f", 0.0));
	TEST_ASSERT_STR("+0", buffer);
	TEST_ASSERT_INT(2, snprintf(buffer, sizeof(buffer), "% .0f", 1.0));
	TEST_ASSERT_STR(" 1", buffer);
	TEST_ASSERT_INT(8, snprintf(buffer, sizeof(buffer), "%08.2f", -1.5));
	TEST_ASSERT_STR("-0001.50", buffer);
}

static void vsnprintf_formats_using_a_forwarded_argument_list(void)
{
	char buffer[32];

	TEST_ASSERT_INT(6, format_with_vsnprintf(buffer, sizeof(buffer),
						 "%s:%d", "port", 7));
	TEST_ASSERT_STR("port:7", buffer);
}

static void vsprintf_formats_using_a_forwarded_argument_list(void)
{
	char buffer[32];

	TEST_ASSERT_INT(6, format_with_vsprintf(buffer, "%#x:%c", 42U, '!'));
	TEST_ASSERT_STR("0x2a:!", buffer);
}

static void snprintf_formats_pointers(void)
{
	char buffer[32];
	int value = 0;

	TEST_ASSERT_INT(5, snprintf(buffer, sizeof(buffer), "%p", NULL));
	TEST_ASSERT_STR("(nil)", buffer);
	TEST_ASSERT_TRUE(snprintf(buffer, sizeof(buffer), "%p", &value) >= 3);
	TEST_ASSERT_TRUE(buffer[0] == '0');
	TEST_ASSERT_TRUE(buffer[1] == 'x');
}

static void fputc_buffers_until_a_newline_in_line_mode(void)
{
	FILE stream = { .fd = 7, .mode = _IOLBF };

	reset_write_capture();
	TEST_ASSERT_INT('a', fputc('a', &stream));
	TEST_ASSERT_SIZE(0, write_length);
	TEST_ASSERT_INT('\n', fputc('\n', &stream));
	TEST_ASSERT_INT(7, write_fd);
	TEST_ASSERT_SIZE(2, write_length);
	TEST_ASSERT_MEM("a\n", write_buffer, write_length);
	TEST_ASSERT_SIZE(0, stream.buf_pos);
}

static void fputc_flushes_an_unbuffered_stream_immediately(void)
{
	FILE stream = { .fd = 8, .mode = _IONBF };

	reset_write_capture();
	TEST_ASSERT_INT('x', fputc('x', &stream));
	TEST_ASSERT_INT(8, write_fd);
	TEST_ASSERT_SIZE(1, write_length);
	TEST_ASSERT_MEM("x", write_buffer, write_length);
	TEST_ASSERT_SIZE(0, stream.buf_pos);
}

static void fflush_writes_buffered_data_and_resets_position(void)
{
	FILE stream = { .fd = 9, .mode = _IOFBF };

	reset_write_capture();
	TEST_ASSERT_INT('a', fputc('a', &stream));
	TEST_ASSERT_INT('b', fputc('b', &stream));
	TEST_ASSERT_SIZE(2, stream.buf_pos);
	TEST_ASSERT_INT(0, fflush(&stream));
	TEST_ASSERT_INT(9, write_fd);
	TEST_ASSERT_SIZE(2, write_length);
	TEST_ASSERT_MEM("ab", write_buffer, write_length);
	TEST_ASSERT_SIZE(0, stream.buf_pos);
}

static void fflush_does_not_write_an_empty_stream(void)
{
	FILE stream = { .fd = 10, .mode = _IOFBF };

	reset_write_capture();
	TEST_ASSERT_INT(0, fflush(&stream));
	TEST_ASSERT_INT(-1, write_fd);
	TEST_ASSERT_SIZE(0, write_length);
}

static void fflush_reports_a_short_write(void)
{
	FILE stream = { .fd = 11, .mode = _IOFBF };

	reset_write_capture();
	TEST_ASSERT_INT('a', fputc('a', &stream));
	write_result = 0;
	TEST_ASSERT_INT(-1, fflush(&stream));
	TEST_ASSERT_SIZE(1, stream.buf_pos);
}

static void vfprintf_propagates_write_errors(void)
{
	FILE stream = { .fd = 18, .mode = _IONBF };

	reset_write_capture();
	write_result = -1;
	int result = fprintf(&stream, "x");
	TEST_ASSERT_INT(-1, result);
}

static void vdprintf_propagates_write_errors(void)
{
	reset_write_capture();
	write_result = -1;
	int result = dprintf(19, "x");
	TEST_ASSERT_INT(-1, result);
}

static void printf_propagates_stdout_errors(void)
{
	FILE saved = *stdout;

	*stdout = (FILE){ .fd = 20, .mode = _IONBF };
	reset_write_capture();
	write_result = -1;
	int result = printf("x");
	*stdout = saved;
	TEST_ASSERT_INT(-1, result);
}

static void vprintf_propagates_stdout_errors(void)
{
	FILE saved = *stdout;

	*stdout = (FILE){ .fd = 21, .mode = _IONBF };
	reset_write_capture();
	write_result = -1;
	int result = format_with_vprintf("x");
	*stdout = saved;
	TEST_ASSERT_INT(-1, result);
}

static void streams_have_expected_default_descriptors_and_modes(void)
{
	TEST_ASSERT_INT(0, stdin->fd);
	TEST_ASSERT_INT(_IONBF, stdin->mode);
	TEST_ASSERT_INT(1, stdout->fd);
	TEST_ASSERT_INT(_IOLBF, stdout->mode);
	TEST_ASSERT_INT(2, stderr->fd);
	TEST_ASSERT_INT(_IOLBF, stderr->mode);
}

static void fputc_reports_write_errors(void)
{
	FILE stream = { .fd = 16, .mode = _IONBF };

	reset_write_capture();
	write_result = -1;
	TEST_ASSERT_INT(-1, fputc('x', &stream));
	TEST_ASSERT_SIZE(0, write_length);
}

static void putchar_reports_write_errors(void)
{
	reset_write_capture();
	write_result = -1;
	TEST_ASSERT_INT(-1, putchar('x'));
	TEST_ASSERT_SIZE(0, write_length);
}

static void fputc_flushes_a_full_buffer(void)
{
	FILE stream = { .fd = 17, .mode = _IOFBF };

	reset_write_capture();
	for (size_t i = 0; i < BUFSIZE; ++i)
		TEST_ASSERT_INT('x', fputc('x', &stream));
	TEST_ASSERT_SIZE(BUFSIZE, write_length);
	TEST_ASSERT_SIZE(0, stream.buf_pos);
}

static void printf_writes_to_stdout_and_returns_the_length(void)
{
	reset_write_capture();

	TEST_ASSERT_INT(7, printf("hello %d", 7));
	TEST_ASSERT_INT(1, write_fd);
	TEST_ASSERT_SIZE(7, write_length);
	TEST_ASSERT_MEM("hello 7", write_buffer, write_length);
}

static void vprintf_writes_to_stdout(void)
{
	reset_write_capture();

	TEST_ASSERT_INT(4, format_with_vprintf("v:%s", "ok"));
	TEST_ASSERT_INT(1, write_fd);
	TEST_ASSERT_MEM("v:ok", write_buffer, write_length);
}

static void fprintf_writes_to_the_given_stream(void)
{
	FILE stream = { .fd = 12, .mode = _IOFBF };

	reset_write_capture();
	TEST_ASSERT_INT(3, fprintf(&stream, "f:%d", 7));
	TEST_ASSERT_INT(12, write_fd);
	TEST_ASSERT_MEM("f:7", write_buffer, write_length);
}

static void vfprintf_writes_to_the_given_stream(void)
{
	FILE stream = { .fd = 13, .mode = _IOLBF };

	reset_write_capture();
	TEST_ASSERT_INT(5, format_with_vfprintf(&stream, "vf:%s", "ok"));
	TEST_ASSERT_INT(13, write_fd);
	TEST_ASSERT_MEM("vf:ok", write_buffer, write_length);
}

static void putchar_writes_one_byte_to_stdout(void)
{
	reset_write_capture();

	TEST_ASSERT_INT('Z', putchar('Z'));
	TEST_ASSERT_INT(1, write_fd);
	TEST_ASSERT_SIZE(1, write_length);
	TEST_ASSERT_MEM("Z", write_buffer, write_length);
}

static void dprintf_writes_to_the_given_descriptor(void)
{
	reset_write_capture();

	TEST_ASSERT_INT(3, dprintf(14, "d:%d", 7));
	TEST_ASSERT_INT(14, write_fd);
	TEST_ASSERT_MEM("d:7", write_buffer, write_length);
}

static void vdprintf_writes_to_the_given_descriptor(void)
{
	reset_write_capture();

	TEST_ASSERT_INT(5, format_with_vdprintf(15, "vd:%s", "ok"));
	TEST_ASSERT_INT(15, write_fd);
	TEST_ASSERT_MEM("vd:ok", write_buffer, write_length);
}

const struct test_case stdio_tests[] = {
	{ "snprintf formats literal text", snprintf_formats_literal_text },
	{ "snprintf formats signed integers",
	  snprintf_formats_signed_integers },
	{ "snprintf formats unsigned integers in each base",
	  snprintf_formats_unsigned_integers_in_each_base },
	{ "snprintf handles integer lengths",
	  snprintf_handles_integer_lengths },
	{ "snprintf applies width and alignment",
	  snprintf_applies_width_and_alignment },
	{ "snprintf applies precision", snprintf_applies_precision },
	{ "snprintf formats strings and characters",
	  snprintf_formats_strings_and_characters },
	{ "snprintf reports required size when truncated",
	  snprintf_reports_required_size_when_truncated },
	{ "sprintf writes an unbounded result",
	  sprintf_writes_an_unbounded_result },
	{ "snprintf supports float formats", snprintf_supports_float_formats },
	{ "snprintf supports special float values",
	  snprintf_supports_special_float_values },
	{ "snprintf accepts dynamic precision",
	  snprintf_accepts_dynamic_precision },
	{ "snprintf combines sign width and zero padding",
	  snprintf_combines_sign_width_and_zero_padding },
	{ "snprintf handles hash zero and negative precision",
	  snprintf_handles_hash_zero_and_negative_precision },
	{ "snprintf handles long and size arguments",
	  snprintf_handles_long_and_size_arguments },
	{ "snprintf handles unknown conversions",
	  snprintf_handles_unknown_conversions_as_literal_text },
	{ "snprintf applies width to floats",
	  snprintf_applies_width_to_floats },
	{ "snprintf handles integer extremes",
	  snprintf_handles_integer_extremes },
	{ "snprintf handles multiple dynamic arguments",
	  snprintf_handles_multiple_dynamic_arguments },
	{ "snprintf handles empty and boundary buffers",
	  snprintf_handles_empty_and_boundary_buffers },
	{ "snprintf handles float rounding boundaries",
	  snprintf_handles_float_rounding_boundaries },
	{ "snprintf handles float sign and zero",
	  snprintf_handles_float_sign_and_zero },
	{ "vsnprintf formats forwarded arguments",
	  vsnprintf_formats_using_a_forwarded_argument_list },
	{ "vsprintf formats forwarded arguments",
	  vsprintf_formats_using_a_forwarded_argument_list },
	{ "snprintf formats pointers", snprintf_formats_pointers },
	{ "fputc buffers line-mode output",
	  fputc_buffers_until_a_newline_in_line_mode },
	{ "fputc flushes unbuffered output immediately",
	  fputc_flushes_an_unbuffered_stream_immediately },
	{ "fflush writes buffered data",
	  fflush_writes_buffered_data_and_resets_position },
	{ "fflush skips empty streams", fflush_does_not_write_an_empty_stream },
	{ "fflush reports short writes", fflush_reports_a_short_write },
	{ "vfprintf propagates write errors",
	  vfprintf_propagates_write_errors },
	{ "vdprintf propagates write errors",
	  vdprintf_propagates_write_errors },
	{ "printf propagates stdout errors", printf_propagates_stdout_errors },
	{ "vprintf propagates stdout errors",
	  vprintf_propagates_stdout_errors },
	{ "streams have expected defaults",
	  streams_have_expected_default_descriptors_and_modes },
	{ "fputc reports write errors", fputc_reports_write_errors },
	{ "putchar reports write errors", putchar_reports_write_errors },
	{ "fputc flushes a full buffer", fputc_flushes_a_full_buffer },
	{ "printf writes to stdout",
	  printf_writes_to_stdout_and_returns_the_length },
	{ "vprintf writes to stdout", vprintf_writes_to_stdout },
	{ "fprintf writes to the given stream",
	  fprintf_writes_to_the_given_stream },
	{ "vfprintf writes to the given stream",
	  vfprintf_writes_to_the_given_stream },
	{ "putchar writes one byte to stdout",
	  putchar_writes_one_byte_to_stdout },
	{ "dprintf writes to the given descriptor",
	  dprintf_writes_to_the_given_descriptor },
	{ "vdprintf writes to the given descriptor",
	  vdprintf_writes_to_the_given_descriptor },
};

const size_t stdio_test_count = sizeof(stdio_tests) / sizeof(stdio_tests[0]);
