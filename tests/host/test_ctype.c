#include "test.h"

#include <ctype.h>

static void classifies_ascii_boundaries(void)
{
	TEST_ASSERT_TRUE(isalpha('A'));
	TEST_ASSERT_TRUE(isalpha('Z'));
	TEST_ASSERT_TRUE(isalpha('a'));
	TEST_ASSERT_TRUE(isalpha('z'));
	TEST_ASSERT_TRUE(!isalpha('@'));
	TEST_ASSERT_TRUE(!isalpha('['));
	TEST_ASSERT_TRUE(!isalpha('`'));
	TEST_ASSERT_TRUE(!isalpha('{'));
	TEST_ASSERT_TRUE(isdigit('0'));
	TEST_ASSERT_TRUE(isdigit('9'));
	TEST_ASSERT_TRUE(!isdigit('/'));
	TEST_ASSERT_TRUE(!isdigit(':'));
	TEST_ASSERT_TRUE(islower('a'));
	TEST_ASSERT_TRUE(islower('z'));
	TEST_ASSERT_TRUE(!islower('A'));
	TEST_ASSERT_TRUE(isupper('A'));
	TEST_ASSERT_TRUE(isupper('Z'));
	TEST_ASSERT_TRUE(!isupper('a'));
}

static void classifies_alphanumeric_and_printable_values(void)
{
	TEST_ASSERT_TRUE(isalnum('A'));
	TEST_ASSERT_TRUE(isalnum('z'));
	TEST_ASSERT_TRUE(isalnum('5'));
	TEST_ASSERT_TRUE(!isalnum(' '));
	TEST_ASSERT_TRUE(!isalnum('_'));
	TEST_ASSERT_TRUE(isprint(' '));
	TEST_ASSERT_TRUE(isprint('~'));
	TEST_ASSERT_TRUE(!isprint('\x1f'));
	TEST_ASSERT_TRUE(!isprint('\x7f'));
}

static void classifies_all_supported_whitespace(void)
{
	const int whitespace[] = { ' ', '\f', '\n', '\r', '\t', '\v' };

	for (size_t i = 0; i < sizeof(whitespace) / sizeof(whitespace[0]); ++i)
		TEST_ASSERT_TRUE(isspace(whitespace[i]));
	TEST_ASSERT_TRUE(!isspace('A'));
	TEST_ASSERT_TRUE(!isspace('\0'));
}

static void handles_values_outside_ascii(void)
{
	TEST_ASSERT_TRUE(isascii(0));
	TEST_ASSERT_TRUE(isascii(127));
	TEST_ASSERT_TRUE(!isascii(-1));
	TEST_ASSERT_TRUE(!isascii(128));
	TEST_ASSERT_TRUE(!isalpha(-1));
	TEST_ASSERT_TRUE(!isdigit(256));
	TEST_ASSERT_TRUE(!isprint(-1));
	TEST_ASSERT_TRUE(!isspace(256));
}

static void classifies_every_ascii_value(void)
{
	for (int c = 0; c <= 127; ++c) {
		int alpha = (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z');
		int digit = c >= '0' && c <= '9';
		int lower = c >= 'a' && c <= 'z';
		int upper = c >= 'A' && c <= 'Z';
		int printable = c >= 32 && c <= 126;
		int space = c == ' ' || c == '\f' || c == '\n' || c == '\r' ||
			    c == '\t' || c == '\v';

		TEST_ASSERT_TRUE(isascii(c));
		TEST_ASSERT_TRUE(isalpha(c) == alpha);
		TEST_ASSERT_TRUE(isalnum(c) == (alpha || digit));
		TEST_ASSERT_TRUE(isdigit(c) == digit);
		TEST_ASSERT_TRUE(islower(c) == lower);
		TEST_ASSERT_TRUE(isupper(c) == upper);
		TEST_ASSERT_TRUE(isprint(c) == printable);
		TEST_ASSERT_TRUE(isspace(c) == space);
	}
}

const struct test_case ctype_tests[] = {
	{ "ctype classifies ASCII boundaries", classifies_ascii_boundaries },
	{ "ctype classifies alphanumeric and printable values",
	  classifies_alphanumeric_and_printable_values },
	{ "ctype classifies all supported whitespace",
	  classifies_all_supported_whitespace },
	{ "ctype handles values outside ASCII", handles_values_outside_ascii },
	{ "ctype classifies every ASCII value", classifies_every_ascii_value },
};

const size_t ctype_test_count = sizeof(ctype_tests) / sizeof(ctype_tests[0]);
