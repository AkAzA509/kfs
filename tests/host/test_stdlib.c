#include "test.h"

#include <limits.h>
#include <stdlib.h>

static void abs_handles_zero_and_regular_values(void)
{
	TEST_ASSERT_INT(0, abs(0));
	TEST_ASSERT_INT(42, abs(42));
	TEST_ASSERT_INT(42, abs(-42));
}

static void abs_handles_the_defined_integer_boundaries(void)
{
	TEST_ASSERT_INT(INT_MAX, abs(INT_MAX));
	TEST_ASSERT_INT(INT_MAX, abs(INT_MIN + 1));
}

static void labs_handles_zero_and_regular_values(void)
{
	TEST_ASSERT_INT(0, labs(0));
	TEST_ASSERT_INT(123456L, labs(123456L));
	TEST_ASSERT_INT(123456L, labs(-123456L));
}

static void labs_handles_the_defined_long_boundaries(void)
{
	TEST_ASSERT_INT(LONG_MAX, labs(LONG_MAX));
	TEST_ASSERT_INT(LONG_MAX, labs(LONG_MIN + 1L));
}

static void ullabs_handles_zero_and_regular_values(void)
{
	TEST_ASSERT_TRUE(ullabs(0) == 0ULL);
	TEST_ASSERT_TRUE(ullabs(9876543210LL) == 9876543210ULL);
	TEST_ASSERT_TRUE(ullabs(-9876543210LL) == 9876543210ULL);
}

static void ullabs_handles_the_defined_long_long_boundaries(void)
{
	TEST_ASSERT_TRUE(ullabs(LLONG_MAX) == (unsigned long long)LLONG_MAX);
	TEST_ASSERT_TRUE(ullabs(LLONG_MIN + 1LL) ==
			 (unsigned long long)LLONG_MAX);
}

const struct test_case stdlib_tests[] = {
	{ "abs handles zero and regular values",
	  abs_handles_zero_and_regular_values },
	{ "abs handles the defined integer boundaries",
	  abs_handles_the_defined_integer_boundaries },
	{ "labs handles zero and regular values",
	  labs_handles_zero_and_regular_values },
	{ "labs handles the defined long boundaries",
	  labs_handles_the_defined_long_boundaries },
	{ "ullabs handles zero and regular values",
	  ullabs_handles_zero_and_regular_values },
	{ "ullabs handles the defined long long boundaries",
	  ullabs_handles_the_defined_long_long_boundaries },
};

const size_t stdlib_test_count = sizeof(stdlib_tests) / sizeof(stdlib_tests[0]);
