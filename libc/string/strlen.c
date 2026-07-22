#include <stdint.h>
#include <string.h>
#include <stddef.h>

size_t strlen(const char* str)
{
	const char *start = str;

	while (IS_ALIGN_PTR(start)) {
		if (!*str)
			return str - start;
		str++;
	}

	const u32_t *s32 = (const u32_t *)str;
	while(IS_NULL_TERM(*s32))
		s32++;

	str = (const char *)s32;
	while(*str)
		str++;

	return str - start;
}

// str = coucou

// val               = 01100011 01101111 01110101 01100011
// val - 0x01010101  = 01100010 01101110 01110100 01100010
// ~val              = 10011100 10010000 10001010 10011100

// (val-0x01010101) & ~val = 00000000 00000000 00000000 00000000

// & 0x80808080 (= 10000000 x4) = 00000000 00000000 00000000 00000000