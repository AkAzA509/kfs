#include "../libc/include/stdio.h"
#include "../libc/include/string.h"
#include "include/test_common.h"

static int g_pass = 0;
static int g_fail = 0;

void t_print(const char *s)
{
	real_write(s, strlen(s));
}

static void t_print_int(int n)
{
	char buf[12];
	int i = 0;
	int neg = (n < 0);
	unsigned int u = neg ? (unsigned int)(-(n + 1)) + 1 : (unsigned int)n;

	if (u == 0)
		buf[i++] = '0';
	while (u > 0) {
		buf[i++] = '0' + (u % 10);
		u /= 10;
	}
	if (neg)
		buf[i++] = '-';
	while (i > 0)
		real_write(&buf[--i], 1);
}

static void checker(const char *fmt, const char *expected, const char *result,
		    int ret, int expct_ret)
{
	if (strcmp(expected, result) || ret != expct_ret) {
		g_fail++;
		t_print("[FAIL] ");
		t_print(fmt);
		t_print("  expect=");
		t_print(expected);
		t_print("  expected len=");
		t_print_int(expct_ret);
		t_print(" | res=");
		t_print(result);
		t_print("  res len=");
		t_print_int(ret);
		t_print("\"\n");
	}
	g_pass++;
	t_print("[OK]   ");
	t_print(fmt);
	t_print("\n");
}

typedef void (*test_runner_t)(const char *expected, const char *fmt, ...);

// --- Runner ---

static void run_sprintf(const char *expected, const char *fmt, ...)
{
	char b_kfs[512] = { 0 };
	va_list a1;

	va_start(a1, fmt);
	int ret = vsprintf(b_kfs, fmt, a1);
	va_end(a1);
	int expct_ret = strlen(expected);
	checker(fmt, expected, b_kfs, ret, expct_ret);
}

static void run_snprintf(const char *expected, const char *fmt, ...)
{
	char b_kfs[512] = { 0 };
	va_list a1;

	va_start(a1, fmt);
	int ret = vsnprintf(b_kfs, sizeof(b_kfs), fmt, a1);
	va_end(a1);
	int expct_ret = strlen(expected);
	checker(fmt, expected, b_kfs, ret, expct_ret);
}

static void run_dprintf(const char *expected, const char *fmt, ...)
{
	stub_clear();
	va_list a1;

	va_start(a1, fmt);
	int ret = vdprintf(1, fmt, a1);
	va_end(a1);
	int expct_ret = strlen(expected);
	checker(fmt, expected, stub_get_written_data(), ret, expct_ret);
}

static void run_fprintf(const char *expected, const char *fmt, ...)
{
	stub_clear();
	va_list a1;

	va_start(a1, fmt);

	int ret = vfprintf(stdout, fmt, a1);
	va_end(a1);
	int expct_ret = strlen(expected);
	fflush(stdout);
	checker(fmt, expected, stub_get_written_data(), ret, expct_ret);
}

static void run_printf(const char *expected, const char *fmt, ...)
{
	stub_clear();
	va_list a1;

	va_start(a1, fmt);
	int ret = vprintf(fmt, a1);
	va_end(a1);
	int expct_ret = strlen(expected);
	const char *res = stub_get_written_data();
	checker(fmt, expected, res, ret, expct_ret);
}

// --- Testing blocs ---

static void test_signed(test_runner_t run)
{
	t_print("\n--- %d / %i ---\n");
	run("0", "%d", 0);
	run("42", "%d", 42);
	run("-42", "%d", -42);
	run("123", "%i", 123);
	run("   42", "%5d", 42);
	run("42   ", "%-5d", 42);
	run("00042", "%05d", 42);
	run("-0042", "%05d", -42);
	run("+42", "%+d", 42);
	run("-42", "%+d", -42);
	run(" 42", "% d", 42);
	run("00042", "%.5d", 42);
	run("", "%.0d", 0);
	run("   00042", "%8.5d", 42);
	run("00042   ", "%-8.5d", 42);
	run("9223372036854775807", "%lld", 9223372036854775807LL);
	run("-9223372036854775808", "%lld",
	    (long long)(-9223372036854775807LL - 1));
	run("-56", "%hhd", (signed char)200);
	run("4464", "%hd", (short)70000);
	run("123456789", "%ld", 123456789L);
	run("-2147483648", "%d", (-2147483647 - 1));
	run("2147483647", "%d", 2147483647);
}

static void test_unsigned_and_hex(test_runner_t run)
{
	t_print("\n--- %u / %x / %X / %o ---\n");
	run("0", "%u", 0u);
	run("4294967295", "%u", 4294967295u);
	run("ff", "%x", 255);
	run("FF", "%X", 255);
	run("0xff", "%#x", 255);
	run("0XFF", "%#X", 255);
	run("000000ff", "%08x", 255);
	run("777", "%o", 511);
	run("0777", "%#o", 511);
}

static void test_string_and_char(test_runner_t run)
{
	t_print("\n--- %s / %c / %p ---\n");
	run("hello KFS", "%s", "hello KFS");
	run("        hi", "%10s", "hi");
	run("hi        ", "%-10s", "hi");
	run("bon", "%.3s", "bonjour");
	run("A", "%c", 'A');
	run("    Z", "%5c", 'Z');
	run("0x12345678", "%p", (void *)0x12345678);
	run("(nil)", "%p", (void *)NULL);
}

static void test_float(test_runner_t run)
{
	t_print("\n--- %f ---\n");
	run("0.000000", "%f", 0.0);
	run("1.500000", "%f", 1.5);
	run("-1.500000", "%f", -1.5);
	run("3.141593", "%f", 3.14159265358979);
	run("3.14", "%.2f", 3.14159265358979);
	run("0.1", "%.1f", 0.15);
	run("0.100000", "%.6f", 0.1);
	run("      3.14", "%10.2f", 3.14159);
	run("3.14      ", "%-10.2f", 3.14159);
	run("0000003.14", "%010.2f", 3.14159);
	run("-000003.14", "%010.2f", -3.14159);
	run("+3.14", "%+.2f", 3.14159);
	run(" 3.14", "% .2f", 3.14159);
	run("100.000000", "%f", 100.0);
	run("0.001000", "%f", 0.001);
	run("0.3333333333", "%.10f", 1.0 / 3.0);
	run("4", "%.0f", 3.5);
	run("1", "%.0f", 0.99);
}

static void test_dynamic(test_runner_t run)
{
	t_print("\n--- Dynamic Width & Precision (*)---\n");
	run("      42", "%*d", 8, 42);
	run("00042", "%.*d", 5, 42);
	run("42      ", "%-*d", 8, 42);
}

static void run_suite(test_runner_t runner, const char *suite_name)
{
	t_print("\n========================================\n");
	t_print(" RUNNING SUITE: ");
	t_print(suite_name);
	t_print("\n========================================\n");

	test_signed(runner);
	test_unsigned_and_hex(runner);
	test_string_and_char(runner);
	test_float(runner);
	test_dynamic(runner);
}

#define GREEN "\033[92m"
#define BLUE "\033[94m"
#define RED "\033[91m"
#define RESET "\033[0m"

void test_printf()
{
	t_print(BLUE "========================================\n");
	t_print(" PRINTF TEST SUITE\n");
	t_print("========================================\n" RESET);

	run_suite(run_printf, "printf");
	run_suite(run_fprintf, "fprintf");
	run_suite(run_dprintf, "dprintf");
	run_suite(run_sprintf, "sprintf");
	run_suite(run_snprintf, "snprintf");

	t_print(g_fail > 0 ? RED : GREEN);
	t_print("\n========================================\n");
	t_print("TOTAL PASS: ");
	t_print_int(g_pass);
	t_print("   TOTAL FAIL: ");
	t_print_int(g_fail);
	t_print("\n========================================\n" RESET);
	// t_print(RESET);
}

int main(void)
{
	g_pass = 0;
	g_fail = 0;

	test_printf();

	return g_fail != 0;
}
