#include "test.h"

#include <string.h>

static void strlen_handles_empty_and_regular_strings(void)
{
	TEST_ASSERT_SIZE(0, strlen(""));
	TEST_ASSERT_SIZE(1, strlen("x"));
	TEST_ASSERT_SIZE(5, strlen("hello"));
	TEST_ASSERT_SIZE(12, strlen("hello world!"));
}

static void strlen_stops_at_the_first_null_byte(void)
{
	const char value[] = { 'a', 'b', '\0', 'c', 'd', '\0' };

	TEST_ASSERT_SIZE(2, strlen(value));
}

static void strlen_handles_unaligned_strings(void)
{
	char buffer[] = "xunaligned string";

	TEST_ASSERT_SIZE(16, strlen(buffer + 1));
}

static void strlen_handles_many_lengths_and_offsets(void)
{
	char buffer[80];

	for (size_t offset = 0; offset < 4; ++offset) {
		for (size_t length = 0; length < 64; ++length) {
			memset(buffer, 'a', sizeof(buffer));
			buffer[offset + length] = '\0';
			TEST_ASSERT_SIZE(length, strlen(buffer + offset));
		}
	}
}

static void strcmp_compares_strings(void)
{
	TEST_ASSERT_INT(0, strcmp("kernel", "kernel"));
	TEST_ASSERT_INT(0, strcmp("", ""));
	TEST_ASSERT_TRUE(strcmp("abc", "abd") < 0);
	TEST_ASSERT_TRUE(strcmp("abd", "abc") > 0);
	TEST_ASSERT_TRUE(strcmp("abc", "abcd") < 0);
	TEST_ASSERT_TRUE(strcmp("abcd", "abc") > 0);
}

static void strcmp_compares_bytes_as_unsigned_values(void)
{
	const char low[] = { (char)0x7f, '\0' };
	const char high[] = { (char)0x80, '\0' };

	TEST_ASSERT_TRUE(strcmp(low, high) < 0);
	TEST_ASSERT_TRUE(strcmp(high, low) > 0);
}

static void strncmp_respects_the_limit(void)
{
	TEST_ASSERT_INT(0, strncmp("abc", "xyz", 0));
	TEST_ASSERT_INT(0, strncmp("kernel", "kernels", 6));
	TEST_ASSERT_TRUE(strncmp("kernel", "kernels", 7) < 0);
	TEST_ASSERT_TRUE(strncmp("abc", "abd", 2) == 0);
	TEST_ASSERT_TRUE(strncmp("abc", "abd", 3) < 0);
	TEST_ASSERT_INT(0, strncmp("different", "strings", 0));
}

static void strncmp_stops_at_a_null_byte(void)
{
	TEST_ASSERT_INT(0, strncmp("abc", "abcXYZ", 3));
	TEST_ASSERT_TRUE(strncmp("abcXYZ", "abc", 6) > 0);
}

static void strncmp_handles_unaligned_strings(void)
{
	char first[] = "xabcdef";
	char second[] = "yabcdef";

	TEST_ASSERT_INT(0, strncmp(first + 1, second + 1, 6));
	TEST_ASSERT_TRUE(strncmp(first + 1, "abcdeg", 6) < 0);
}

static void memset_fills_the_requested_bytes(void)
{
	unsigned char buffer[8] = { 0 };

	memset(buffer, 0xa5, sizeof(buffer));
	for (size_t i = 0; i < sizeof(buffer); ++i)
		TEST_ASSERT_INT(0xa5, buffer[i]);
}

static void memset_truncates_the_value_to_one_byte(void)
{
	unsigned char buffer[4] = { 0 };

	memset(buffer, 0x1234, sizeof(buffer));
	for (size_t i = 0; i < sizeof(buffer); ++i)
		TEST_ASSERT_INT(0x34, buffer[i]);
}

static void memset_preserves_bytes_outside_the_range(void)
{
	unsigned char buffer[] = { 0x11, 0x22, 0x33, 0x44 };
	const unsigned char expected[] = { 0x11, 0xaa, 0xaa, 0x44 };

	memset(buffer + 1, 0xaa, 2);
	TEST_ASSERT_MEM(expected, buffer, sizeof(buffer));
}

static void memset_accepts_a_zero_length(void)
{
	unsigned char buffer[] = { 0x11, 0x22 };
	const unsigned char expected[] = { 0x11, 0x22 };

	TEST_ASSERT_TRUE(memset(buffer, 0xaa, 0) == buffer);
	TEST_ASSERT_MEM(expected, buffer, sizeof(buffer));
}

static void memcpy_copies_bytes_and_returns_destination(void)
{
	const unsigned char source[] = { 0x00, 0x11, 0x80, 0xff };
	unsigned char destination[sizeof(source)] = { 0 };

	TEST_ASSERT_TRUE(memcpy(destination, source, sizeof(source)) ==
			 destination);
	TEST_ASSERT_MEM(source, destination, sizeof(source));
}

static void memcpy_copies_unaligned_ranges(void)
{
	const char source[] = "copy this";
	char destination[sizeof(source) + 2] = { 0 };

	TEST_ASSERT_TRUE(memcpy(destination + 1, source, sizeof(source)) ==
			 destination + 1);
	TEST_ASSERT_STR(source, destination + 1);
}

static void memcpy_accepts_a_zero_length(void)
{
	unsigned char source[] = { 0x01, 0x02 };
	unsigned char destination[] = { 0xaa, 0xbb };
	const unsigned char expected[] = { 0xaa, 0xbb };

	TEST_ASSERT_TRUE(memcpy(destination, source, 0) == destination);
	TEST_ASSERT_MEM(expected, destination, sizeof(destination));
}

static void memmove_handles_overlapping_regions(void)
{
	char buffer[] = "0123456789";

	TEST_ASSERT_TRUE(memmove(buffer + 2, buffer, 8) == buffer + 2);
	TEST_ASSERT_STR("0101234567", buffer);

	memcpy(buffer, "0123456789", sizeof(buffer));
	memmove(buffer, buffer + 2, 8);
	TEST_ASSERT_STR("2345678989", buffer);
}

static void memmove_handles_a_one_byte_overlap(void)
{
	char buffer[] = "0123456789";

	memmove(buffer + 1, buffer, 9);
	TEST_ASSERT_STR("0012345678", buffer);
}

static void memmove_handles_same_source_and_destination(void)
{
	char buffer[] = "unchanged";

	TEST_ASSERT_TRUE(memmove(buffer, buffer, sizeof(buffer)) == buffer);
	TEST_ASSERT_STR("unchanged", buffer);
}

static void memmove_accepts_a_zero_length(void)
{
	char buffer[] = "unchanged";

	TEST_ASSERT_TRUE(memmove(buffer + 1, buffer, 0) == buffer + 1);
	TEST_ASSERT_STR("unchanged", buffer);
}

static void memcmp_compares_bytes_as_unsigned_values(void)
{
	const unsigned char low[] = { 0x01, 0x7f };
	const unsigned char high[] = { 0x01, 0x80 };

	TEST_ASSERT_INT(0, memcmp(low, low, sizeof(low)));
	TEST_ASSERT_TRUE(memcmp(low, high, sizeof(low)) < 0);
	TEST_ASSERT_TRUE(memcmp(high, low, sizeof(low)) > 0);
	TEST_ASSERT_INT(0, memcmp(low, high, 1));
}

static void memcmp_only_compares_the_requested_prefix(void)
{
	const char first[] = "prefix-one";
	const char second[] = "prefix-two";

	TEST_ASSERT_INT(0, memcmp(first, second, 7));
	TEST_ASSERT_TRUE(memcmp(first, second, sizeof(first)) < 0);
}

static void memcmp_accepts_a_zero_length(void)
{
	const unsigned char first[] = { 0xff };
	const unsigned char second[] = { 0x00 };

	TEST_ASSERT_INT(0, memcmp(first, second, 0));
}

static void memory_functions_handle_many_lengths_and_offsets(void)
{
	unsigned char source[80];
	unsigned char destination[80];

	for (size_t i = 0; i < sizeof(source); ++i)
		source[i] = (unsigned char)(i * 3);

	for (size_t offset = 0; offset < 4; ++offset) {
		for (size_t length = 0; length < 48; ++length) {
			memset(destination, 0, sizeof(destination));
			TEST_ASSERT_TRUE(memcpy(destination + offset,
						source + offset, length) ==
					 destination + offset);
			TEST_ASSERT_MEM(source + offset, destination + offset,
					length);
			TEST_ASSERT_INT(0,
					memcmp(source + offset,
					       destination + offset, length));
		}
	}
}

static void memmove_handles_many_overlapping_lengths(void)
{
	const char source[] = "0123456789abcdef";

	for (size_t shift = 1; shift < 8; ++shift) {
		char forward[32];
		char backward[32];

		memcpy(forward, source, sizeof(source));
		memcpy(backward, source, sizeof(source));

		memmove(forward + shift, forward, 16 - shift);
		for (size_t i = 0; i < 16 - shift; ++i)
			TEST_ASSERT_INT(source[i], forward[shift + i]);

		memmove(backward, backward + shift, 16 - shift);
		for (size_t i = 0; i < 16 - shift; ++i)
			TEST_ASSERT_INT(source[shift + i], backward[i]);
	}
}

static void trim_removes_whitespace_in_place(void)
{
	char value[] = " \t hello world \n ";
	char *trimmed = trim(value);

	TEST_ASSERT_TRUE(trimmed == value + 3);
	TEST_ASSERT_STR("hello world", trimmed);
	TEST_ASSERT_INT(' ', value[0]);
	TEST_ASSERT_INT('\0', value[14]);
}

static void trim_handles_empty_and_unpadded_strings(void)
{
	char empty[] = "";
	char value[] = "kernel";

	TEST_ASSERT_TRUE(trim(empty) == empty);
	TEST_ASSERT_TRUE(trim(value) == value);
	TEST_ASSERT_STR("kernel", value);
}

static void trim_handles_a_string_containing_only_whitespace(void)
{
	char value[] = " \t\n\r";
	char *trimmed = trim(value);

	TEST_ASSERT_TRUE(*trimmed == '\0');
	TEST_ASSERT_TRUE(trimmed == value + 4);
}

static void trim_preserves_internal_whitespace(void)
{
	char value[] = "  hello \t world  ";

	TEST_ASSERT_STR("hello \t world", trim(value));
}

const struct test_case string_tests[] = {
	{ "strlen handles empty and regular strings",
	  strlen_handles_empty_and_regular_strings },
	{ "strlen stops at the first null byte",
	  strlen_stops_at_the_first_null_byte },
	{ "strlen handles unaligned strings",
	  strlen_handles_unaligned_strings },
	{ "strlen handles many lengths and offsets",
	  strlen_handles_many_lengths_and_offsets },
	{ "strcmp compares strings", strcmp_compares_strings },
	{ "strcmp compares bytes as unsigned values",
	  strcmp_compares_bytes_as_unsigned_values },
	{ "strncmp respects the limit", strncmp_respects_the_limit },
	{ "strncmp stops at a null byte", strncmp_stops_at_a_null_byte },
	{ "strncmp handles unaligned strings",
	  strncmp_handles_unaligned_strings },
	{ "memset fills the requested bytes",
	  memset_fills_the_requested_bytes },
	{ "memset truncates the value to one byte",
	  memset_truncates_the_value_to_one_byte },
	{ "memset preserves bytes outside the range",
	  memset_preserves_bytes_outside_the_range },
	{ "memset accepts a zero length", memset_accepts_a_zero_length },
	{ "memcpy copies bytes and returns destination",
	  memcpy_copies_bytes_and_returns_destination },
	{ "memcpy copies unaligned ranges", memcpy_copies_unaligned_ranges },
	{ "memcpy accepts a zero length", memcpy_accepts_a_zero_length },
	{ "memmove handles overlapping regions",
	  memmove_handles_overlapping_regions },
	{ "memmove handles a one byte overlap",
	  memmove_handles_a_one_byte_overlap },
	{ "memmove handles same source and destination",
	  memmove_handles_same_source_and_destination },
	{ "memmove accepts a zero length", memmove_accepts_a_zero_length },
	{ "memcmp compares bytes as unsigned values",
	  memcmp_compares_bytes_as_unsigned_values },
	{ "memcmp only compares the requested prefix",
	  memcmp_only_compares_the_requested_prefix },
	{ "memcmp accepts a zero length", memcmp_accepts_a_zero_length },
	{ "memory functions handle many lengths and offsets",
	  memory_functions_handle_many_lengths_and_offsets },
	{ "memmove handles many overlapping lengths",
	  memmove_handles_many_overlapping_lengths },
	{ "trim removes whitespace in place",
	  trim_removes_whitespace_in_place },
	{ "trim handles empty and unpadded strings",
	  trim_handles_empty_and_unpadded_strings },
	{ "trim handles whitespace-only strings",
	  trim_handles_a_string_containing_only_whitespace },
	{ "trim preserves internal whitespace",
	  trim_preserves_internal_whitespace },
};

const size_t string_test_count = sizeof(string_tests) / sizeof(string_tests[0]);
