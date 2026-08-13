#include <stdio.h>
#include <string.h>
#include "include/test_common.h"

static void t_print(const char *s)
{
	while (*s)
		putchar(*s++);
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
		putchar(buf[--i]);
}

static int g_pass = 0;
static int g_fail = 0;

typedef void (*test_runner_t)(const char *expected, const char *fmt, ...);

// --- Runner ---

static void run_sprintf(const char *expected, const char *fmt, ...)
{
	char b_kfs[512] = { 0 };
	va_list a1;

	va_start(a1, fmt);
	int r_kfs = vsprintf(b_kfs, fmt, a1);
	va_end(a1);

	assert(expected, );
}

static void run_snprintf(const char *expected, const char *fmt, ...)
{
	char b_kfs[512] = { 0 };
	va_list a1;

	va_start(a1, fmt);
	int r_kfs = vsnprintf(b_kfs, sizeof(b_kfs), fmt, a1);
	va_end(a1);

	assert();
}

static void run_dprintf(const char *expected, const char *fmt, ...)
{
	char b_kfs[512] = { 0 };
	va_list a1;

	va_start(a1, fmt);
	int r_kfs = vdprintf(1, fmt, a1);
	va_end(a1);

	assert();
}

static void run_fprintf(const char *expected, const char *fmt, ...)
{
	char b_kfs[512] = { 0 };

	va_list a1;
	va_start(a1, fmt);

	int r_kfs = vfprintf(stdout, fmt, a1);

	va_end(a1);

	assert();
}

static void run_printf(const char *expected, const char *fmt, ...)
{
	char b_kfs[512] = { 0 };
	va_list a1;

	va_start(a1, fmt);
	int r_kfs = vprintf(fmt, a1);

	va_end(a1);

	assert(expected);
}

// --- Testing blocs ---

static void test_signed(test_runner_t run)
{
	t_print("\n--- %d / %i ---\n");
	run("%d", 0);
	run("%d", 42);
	run("%d", -42);
	run("%i", 123);
	run("%5d", 42);
	run("%-5d", 42);
	run("%05d", 42);
	run("%05d", -42);
	run("%+d", 42);
	run("%+d", -42);
	run("% d", 42);
	run("%.5d", 42);
	run("%.0d", 0);
	run("%8.5d", 42);
	run("%-8.5d", 42);
	run("%lld", 9223372036854775807LL);
	run("%lld", (long long)(-9223372036854775807LL - 1));
	run("%hhd", (signed char)200);
	run("%hd", (short)70000);
	run("%ld", 123456789L);
	run("%d", (-2147483647 - 1));
	run("%d", 2147483647);
}

static void test_unsigned_and_hex(test_runner_t run)
{
	t_print("\n--- %u / %x / %X / %o ---\n");
	run("%u", 0u);
	run("%u", 4294967295u);
	run("%x", 255);
	run("%X", 255);
	run("%#x", 255);
	run("%#X", 255);
	run("%08x", 255);
	run("%o", 511);
	run("%#o", 511);
}

static void test_string_and_char(test_runner_t run)
{
	t_print("\n--- %s / %c / %p ---\n");
	run("%s", "hello KFS");
	run("%10s", "hi");
	run("%-10s", "hi");
	run("%.3s", "bonjour");
	run("%c", 'A');
	run("%5c", 'Z');
	run("%p", (void *)0x12345678);
	run("%p", (void *)NULL);
}

static void test_float(test_runner_t run)
{
	t_print("\n--- %f ---\n");
	run("%f", 0.0);
	run("%f", 1.5);
	run("%f", -1.5);
	run("%f", 3.14159265358979);
	run("%.2f", 3.14159265358979);
	run("%.1f", 0.15);
	run("%.6f", 0.1);
	run("%10.2f", 3.14159);
	run("%-10.2f", 3.14159);
	run("%010.2f", 3.14159);
	run("%010.2f", -3.14159);
	run("%+.2f", 3.14159);
	run("% .2f", 3.14159);
	run("%f", 100.0);
	run("%f", 0.001);
	run("%.10f", 1.0 / 3.0);
	run("%.0f", 3.5);
	run("%.0f", 0.99);
}

static void test_dynamic(test_runner_t run)
{
	t_print("\n--- Dynamic Width & Precision (*)---\n");
	run("%*d", 8, 42);
	run("%.*d", 5, 42);
	run("%-*d", 8, 42);
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
	// t_print(RESET);
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