#ifndef STDIO_H
#define STDIO_H

// #include <stdint.h>
#include <stddef.h>
#include <stdarg.h>

#define BUFSIZ 1024

#define _IOFBF 0 // Full buffering (flush when the buffer is full)
#define _IOLBF 1 // Line buffering (flush each '\n')
#define _IONBF 2 // No buffering

typedef struct _IO_FILE {
	int fd;					// Descripteur UNIX sous-jacent
	char buffer[BUFSIZ];	// Buffer mémoire
	size_t buf_pos;			// Position actuelle dans le buffer
	int mode;				// _IOLBF, _IONBF, etc.
} FILE;

extern FILE *stdout;
extern FILE *stdin;
extern FILE *stderr;

// --- Function declaration ---

// fputc() writes the character c, cast to an unsigned char,
// to stream.
int fputc(int c, FILE *stream);

// For output streams, fflush() forces a write of all user-space
// buffered data for the given output or update stream via the
// stream's underlying write function.
int fflush(FILE *stream);

// putchar(c) writes the character c to the standart output (stdout).
int putchar(int c);

// The printf() function produces output according to format, writing to
// standard output. The format string contains conversion specifications
// beginning with '%', which are replaced by the corresponding argument.
int printf(const char *restrict fmt, ...) __attribute__((format(printf, 1, 2)));

// The vprintf() function is equivalent to printf(), except that it is
// called with a va_list instance instead of a variable number of
// arguments, allowing another variadic function to forward its own
// arguments to it.
int vprintf(const char *restrict fmt, va_list ap)
	__attribute__((format(printf, 1, 0)));

// The fprintf() function produces output according to format, writing to
// the given stream instead of standard output.
int fprintf(FILE *restrict stream, const char *restrict fmt, ...)
	__attribute__((nonnull(1), format(printf, 2, 3)));

// The vfprintf() function is equivalent to fprintf(), except that it is
// called with a va_list instance instead of a variable number of
// arguments.
int vfprintf(FILE *restrict stream, const char *restrict fmt, va_list ap)
	__attribute__((nonnull(1), format(printf, 2, 0)));

// The dprintf() function produces output according to format, writing
// directly to the file descriptor fd instead of a buffered FILE stream.
int dprintf(int fd, const char *restrict fmt, ...)
	__attribute__((format(printf, 2, 3)));

// The vdprintf() function is equivalent to dprintf(), except that it is
// called with a va_list instance instead of a variable number of
// arguments.
int vdprintf(int fd, const char *restrict fmt, va_list ap)
	__attribute__((format(printf, 2, 0)));

// The sprintf() function produces output according to format, writing the
// result into the buffer str. The caller must ensure str is large enough;
// no bounds checking is performed. Prefer snprintf() when the output size
// is not known in advance.
int sprintf(char *restrict str, const char *restrict fmt, ...)
	__attribute__((nonnull(1), format(printf, 2, 3)));

// The vsprintf() function is equivalent to sprintf(), except that it is
// called with a va_list instance instead of a variable number of
// arguments.
int vsprintf(char *restrict str, const char *restrict fmt, va_list ap)
	__attribute__((nonnull(1), format(printf, 2, 0)));

// The snprintf() function produces output according to format, writing at
// most size - 1 characters into str, followed by a terminating null byte.
// The return value is the number of characters that would have been
// written had size been unlimited, allowing the caller to detect
// truncation when the return value is >= size.
int snprintf(char *restrict str, size_t size, const char *restrict fmt, ...)
	__attribute__((format(printf, 3, 4)));

// The vsnprintf() function is equivalent to snprintf(), except that it is
// called with a va_list instance instead of a variable number of
// arguments.
int vsnprintf(char *restrict str, size_t size, const char *restrict fmt,
	      va_list ap) __attribute__((format(printf, 3, 0)));

#endif // STDIO_H