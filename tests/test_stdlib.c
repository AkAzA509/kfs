#include "../libc/include/limits.h"
#include "../libc/include/stdlib.h"
#include "include/test_common.h"

static int stdlib_pass = 0;
static int stdlib_fail = 0;

static void checker_int(int val, int ret, int expected)
{
	if (ret == expected) {
		stdlib_pass++;
		real_printf("  [OK]   %d\n", val);
	} else {
		stdlib_fail++;
		real_printf("  [FAIL] %d  expected=%d res=%d\n", val, expected,
			    ret);
	}
}

static void checker_long(long val, long ret, long expected)
{
	if (ret == expected) {
		stdlib_pass++;
		real_printf("  [OK]   %ld\n", val);
	} else {
		stdlib_fail++;
		real_printf("  [FAIL] %ld  expected=%ld res=%ld\n", val,
			    expected, ret);
	}
}

static void checker_ullong(long long val, unsigned long long ret,
			   unsigned long long expected)
{
	if (ret == expected) {
		stdlib_pass++;
		real_printf("  [OK]   %lld\n", val);
	} else {
		stdlib_fail++;
		real_printf("  [FAIL] %lld  expected=%llu res=%llu\n", val,
			    expected, ret);
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

static void run_ullabs()
{
	print_func_name("ullabs");

	long long val = 0;
	checker_ullong(val, ullabs(val), 0);

	val = -0;
	checker_ullong(val, ullabs(val), 0);

	val = 1;
	checker_ullong(val, ullabs(val), 1);

	val = -1;
	checker_ullong(val, ullabs(val), 1);

	val = LONG_MIN;
	checker_ullong(val, ullabs(val), 2147483648);

	val = LONG_MAX;
	checker_ullong(val, ullabs(val), 2147483647);

	long long val2 = LLONG_MAX + 10;
	checker_ullong(val2, ullabs(val2), 9223372036854775799);

	val2 = LLONG_MIN - 10;
	checker_ullong(val2, ullabs(val2), 9223372036854775798);

	val = -123456789;
	checker_ullong(val, ullabs(val), 123456789);

	val = 987654321;
	checker_ullong(val, ullabs(val), 987654321);

	val = 42;
	checker_ullong(val, ullabs(val), 42);
}

void test_stdlib()
{
	stdlib_pass = 0;
	stdlib_fail = 0;

	print_suite_name("ABS");

	run_abs();
	run_labs();
	run_ullabs();
}

void get_stdlib_score(int *pass, int *fail, int *total)
{
	*pass = stdlib_pass;
	*fail = stdlib_fail;
	*total = stdlib_pass + stdlib_fail;
}
