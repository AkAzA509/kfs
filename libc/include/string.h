#ifndef STRING_H
#define STRING_H

#include <stddef.h>

#define IS_NULL_TERM(x) (((x) - 0x01010101) & ~(x) & 0x80808080)
#define IS_ALIGN_PTR(x) ((uintptr_t)(x) & (sizeof(uintptr_t) - 1))
#define UNALIGNED_X_Y(x, y) \
	(((long)(x) & (sizeof(long) - 1)) | ((long)(y) & (sizeof(long) - 1)))

// The strlen() function calculates the length of the string pointed to
// by str, excluding the terminating null byte ('\0').
[[gnu::nonnull(1)]] size_t strlen(const char *str) [[reproducible]];

// The strncmp() function compares the two strings s1 and s2,
// except it compares only the first (at most) n bytes of s1 and s2.
[[gnu::nonnull(1, 2)]] int strncmp(const char *s1, const char *s2, size_t n);

// The strcmp() function compares the two strings s1 and s2,
// the comparison is done using unsigned characters.
[[gnu::nonnull(1, 2)]] int strcmp(const char *s1, const char *s2);

// The memset() function fills the first len bytes of the memory area
// pointed to by ptr with the constant byte c.
[[gnu::nonnull(1)]] void *memset(void *ptr, int c, size_t len);

// The memcpy() function copies len bytes from memory area src to memory
// area dst. The memory areas must not overlap. Use memmove(3) if the
// memory areas do overlap.
[[gnu::nonnull(1, 2)]] void *memcpy(void *dst, const void *src, size_t len);

// The memmove() function copies len bytes from memory area src to memory
// area dst. The memory areas may overlap: copying takes place as
// though the bytes in src are first copied into a temporary array that
// does not overlap src or dest, and the bytes are then copied from the
// temporary array to dst.
[[gnu::nonnull(1, 2)]] void *memmove(void *dst, const void *src, size_t len);

// The memcmp() function compares the first len bytes (each interpreted as
// unsigned char) of the memory areas s1 and s2.
[[gnu::nonnull(1, 2)]] int memcmp(const void *s1, const void *s2, size_t len);

#endif // STRING_H
