#include "test.h"

extern const struct test_case string_tests[];
extern const size_t string_test_count;
extern const struct test_case ctype_tests[];
extern const size_t ctype_test_count;
extern const struct test_case stdlib_tests[];
extern const size_t stdlib_test_count;
extern const struct test_case stdio_tests[];
extern const size_t stdio_test_count;

int main(void)
{
	int result = 0;

	result |= test_run(string_tests, string_test_count);
	result |= test_run(ctype_tests, ctype_test_count);
	result |= test_run(stdlib_tests, stdlib_test_count);
	result |= test_run(stdio_tests, stdio_test_count);
	return result;
}
