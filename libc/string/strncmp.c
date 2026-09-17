#include <stddef.h>
#include <string.h>

[[gnu::nonnull(1, 2)]] int strncmp(const char *s1, const char *s2, size_t n)
{
	while (n > 0 && (IS_ALIGN_PTR(s1) || IS_ALIGN_PTR(s2))) {
		unsigned char c1 = (unsigned char)*s1;
		unsigned char c2 = (unsigned char)*s2;

		if (c1 != c2)
			return c1 - c2;
		if (c1 == '\0')
			return 0;
		s1++;
		s2++;
		n--;
	}

	while (n >= sizeof(u32_t)) {
		const u32_t word1 = *(const u32_t *)s1;
		const u32_t word2 = *(const u32_t *)s2;

		if (word1 != word2 || HAS_ZERO_BYTE(word1))
			break;

		s1 += sizeof(word1);
		s2 += sizeof(word2);
		n -= sizeof(word1);
	}

	for (size_t i = 0; i < n; ++i) {
		unsigned char c1 = (unsigned char)s1[i];
		unsigned char c2 = (unsigned char)s2[i];

		if (c1 != c2)
			return c1 - c2;
		if (c1 == '\0')
			return 0;
	}

	return 0;
}