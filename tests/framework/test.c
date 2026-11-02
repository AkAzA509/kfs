#include "test.h"

#include <stdarg.h>
#include <stdio.h>

static void test_output(const char *format, ...)
{
	char buffer[512];
	va_list ap;

	va_start(ap, format);
	int length = vsnprintf(buffer, sizeof(buffer), format, ap);
	va_end(ap);
	if (length <= 0)
		return;
	if (length >= (int)sizeof(buffer))
		length = sizeof(buffer) - 1;

	int result;
	__asm__ volatile("int $0x80"
			 : "=a"(result)
			 : "a"(4), "b"(2), "c"(buffer), "d"(length)
			 : "memory");
	(void)result;
}

static const char *current_test;
static int current_failed;
static size_t passed_tests;
static size_t failed_tests;

void test_begin(const char *name)
{
	current_test = name;
	current_failed = 0;
}

void test_fail(const char *file, int line, const char *message)
{
	if (!current_failed)
		test_output("  FAIL %s:%d: %s\n", file, line, message);
	current_failed = 1;
}

int test_end(void)
{
	if (current_failed) {
		++failed_tests;
		return 0;
	}

	++passed_tests;
	test_output("  PASS %s\n", current_test);
	return 1;
}

int test_run(const struct test_case *tests, size_t count)
{
	test_output("Running %zu tests\n", count);

	for (size_t i = 0; i < count; ++i) {
		test_begin(tests[i].name);
		tests[i].run();
		test_end();
	}

	test_output("Result: %zu passed, %zu failed\n", passed_tests,
		    failed_tests);
	return failed_tests == 0 ? 0 : 1;
}
