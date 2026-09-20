#include "include/test_common.h"
#include "../../libc/include/string.h"

void t_print(const char *s)
{
	real_write(s, strlen(s));
}

void t_print_int(int n)
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

void print_suite_name(const char *suite)
{
	t_print(BLUE "========================================\n");
	t_print(" ");
	t_print(suite);
	t_print(" TEST SUITE\n");
	t_print("========================================\n" RESET);
}

void print_func_name(const char *func)
{
	t_print("\n========================================\n");
	t_print(" RUNNING TEST: ");
	t_print(func);
	t_print("\n========================================\n");
}

void print_result(int pass, int fail)
{
	t_print(fail > 0 ? RED : GREEN);
	t_print("\n========================================\n");
	t_print("TOTAL PASS: ");
	t_print_int(pass);
	t_print("   TOTAL FAIL: ");
	t_print_int(fail);
	t_print("\n========================================\n" RESET);
}