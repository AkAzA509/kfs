#include "../libc/include/stdlib.h"
#include "../libc/include/limits.h"
#include "include/test_common.h"

static int g_pass = 0;
static int g_fail = 0;

static void checker_int(int val, int ret, int expected)
{
	if (ret == expected) {
		g_pass++;
		t_print("[OK]   ");
		t_print_int(val);
		t_print("\n");
	} else {
		g_fail++;
		t_print("[FAIL] ");
		t_print_int(val);
		t_print("  expected=");
		t_print_int(expected);
		t_print(" res=");
		t_print_int(ret);
		t_print("\n");
	}
}

static void checker_long(long val, long ret, long expected)
{
	if (ret == expected) {
		g_pass++;
		t_print("[OK]   ");
		t_print_int(val);
		t_print("\n");
	} else {
		g_fail++;
		t_print("[FAIL] ");
		t_print_int(val);
		t_print("  expected=");
		t_print_int(expected);
		t_print(" res=");
		t_print_int(ret);
		t_print("\n");
	}
}

static void run_abs()
{
	print_func_name("abs");

	int val = 0;
	checker_int(val, abs(val), 0);

	val = -0;
	checker_int(val, abs(val), 0);

	val = 1;
	checker_int(val, abs(val), 1);

	val = -1;
	checker_int(val, abs(val), 1);

	val = INT_MIN;
	checker_int(val, abs(val), -2147483648);

	val = INT_MAX;
	checker_int(val, abs(val), 2147483647);

	int long long val2 = INT_MAX + 10;
	checker_int(val2, abs(val2), 2147483639);

	val2 = INT_MIN - 10;
	checker_int(val2, abs(val2), 2147483638);

	val = -123456789;
	checker_int(val, abs(val), 123456789);

	val = 987654321;
	checker_int(val, abs(val), 987654321);

	val = 42;
	checker_int(val, abs(val), 42);
}

static void run_labs()
{
	print_func_name("labs");

	long val = 0;
	checker_long(val, labs(val), 0);

	val = -0;
	checker_long(val, labs(val), 0);

	val = 1;
	checker_long(val, labs(val), 1);

	val = -1;
	checker_long(val, labs(val), 1);

	val = LONG_MIN;
	checker_long(val, labs(val), -2147483648);

	val = LONG_MAX;
	checker_long(val, labs(val), 2147483647);

	int long long val2 = LONG_MAX + 10;
	checker_long(val2, labs(val2), 2147483639);

	val2 = LONG_MIN - 10;
	checker_long(val2, labs(val2), 2147483638);

	val = -123456789;
	checker_long(val, labs(val), 123456789);

	val = 987654321;
	checker_long(val, labs(val), 987654321);

	val = 42;
	checker_long(val, labs(val), 42);
}

static void test_abs()
{
	print_suite_name("ABS");

	run_abs();
	run_labs();

	print_result(g_pass, g_fail);
}

int main()
{
	g_pass = 0;
	g_fail = 0;

	test_abs();
	return g_fail != 0;
}