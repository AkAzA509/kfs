#ifndef TEST_H
#define TEST_H

#include <stddef.h>

typedef void (*test_fn)(void);

struct test_case {
	const char *name;
	test_fn run;
};

void test_begin(const char *name);
void test_fail(const char *file, int line, const char *message);
int test_end(void);
int test_run(const struct test_case *tests, size_t count);

#define TEST_ASSERT_TRUE(condition)                                \
	do {                                                       \
		if (!(condition)) {                                \
			test_fail(__FILE__, __LINE__, #condition); \
			return;                                    \
		}                                                  \
	} while (0)

#define TEST_ASSERT_INT(expected, actual)                    \
	do {                                                 \
		long test_expected = (long)(expected);       \
		long test_actual = (long)(actual);           \
		if (test_expected != test_actual) {          \
			test_fail(__FILE__, __LINE__,        \
				  #expected " == " #actual); \
			return;                              \
		}                                            \
	} while (0)

#define TEST_ASSERT_SIZE(expected, actual)                   \
	do {                                                 \
		size_t test_expected = (expected);           \
		size_t test_actual = (actual);               \
		if (test_expected != test_actual) {          \
			test_fail(__FILE__, __LINE__,        \
				  #expected " == " #actual); \
			return;                              \
		}                                            \
	} while (0)

#define TEST_ASSERT_MEM(expected, actual, length)                           \
	do {                                                                \
		const unsigned char *test_expected =                        \
			(const unsigned char *)(expected);                  \
		const unsigned char *test_actual =                          \
			(const unsigned char *)(actual);                    \
		size_t test_length = (length);                              \
		size_t test_i;                                              \
		for (test_i = 0; test_i < test_length; ++test_i) {          \
			if (test_expected[test_i] != test_actual[test_i]) { \
				test_fail(__FILE__, __LINE__,               \
					  #expected " == " #actual);        \
				return;                                     \
			}                                                   \
		}                                                           \
	} while (0)

#define TEST_ASSERT_STR(expected, actual)                            \
	do {                                                         \
		const char *test_expected = (expected);              \
		const char *test_actual = (actual);                  \
		size_t test_i = 0;                                   \
		while (test_expected[test_i] != '\0' &&              \
		       test_expected[test_i] == test_actual[test_i]) \
			++test_i;                                    \
		if (test_expected[test_i] != test_actual[test_i]) {  \
			test_fail(__FILE__, __LINE__,                \
				  #expected " == " #actual);         \
			return;                                      \
		}                                                    \
	} while (0)

#endif
