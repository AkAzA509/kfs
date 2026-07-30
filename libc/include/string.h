#ifndef STRING_H
#define STRING_H

#include <stddef.h>

#define IS_NULL_TERM(x) (((x) - 0x01010101) & ~(x) & 0x80808080)
#define IS_ALIGN_PTR(x) ((uintptr_t)(x) & (sizeof(uintptr_t) - 1))
#define UNALIGNED_X_Y(X, Y) \
	(((long)X & (sizeof(long) - 1)) | ((long)Y & (sizeof(long) - 1)))

// The strlen() function calculates the length of the string pointed to
// by str, excluding the terminating null byte ('\0').
size_t strlen(const char *str) __attribute__((
	nonnull(1),
	pure)); // warning, pure tell this function read only its memry, as no call to another function
// and the compiler optimise it to avoid call with the same value

// The strncmp() function compares the two strings s1 and s2,
// except it compares only the first (at most) n bytes of s1 and s2.
int strncmp(const char *s1, const char *s2, size_t n)
	__attribute__((nonnull(1, 2)));

// The strcmp() function compares the two strings s1 and s2,
// the comparison is done using unsigned characters.
int strcmp(const char *s1, const char *s2) __attribute__((nonnull(1, 2)));

// The memset() function fills the first len bytes of the memory area
// pointed to by ptr with the constant byte c.
void *memset(void *ptr, int c, size_t len) __attribute__((nonnull(1)));

// The memcpy() function copies len bytes from memory area src to memory
// area dst. The memory areas must not overlap. Use memmove(3) if the
// memory areas do overlap.
void *memcpy(void *dst, const void *src, size_t len)
	__attribute__((nonnull(1, 2)));

// The memmove() function copies len bytes from memory area src to memory
// area dst. The memory areas may overlap: copying takes place as
// though the bytes in src are first copied into a temporary array that
// does not overlap src or dest, and the bytes are then copied from the
// temporary array to dst.
void *memmove(void *dst, const void *src, size_t len)
	__attribute__((nonnull(1, 2)));

// The memcmp() function compares the first len bytes (each interpreted as
// unsigned char) of the memory areas s1 and s2.
int memcmp(const void *s1, const void *s2, size_t len)
	__attribute__((nonnull(1, 2)));

#endif // STRING_H
