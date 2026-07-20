#ifndef STDIO_H
#define STDIO_H

#include <stdint.h>
#include <stddef.h>
#include <stdarg.h>

int printf(const char *restrict fmt, ...);
int vprintf(const char *restrict fmt, va_list ap);

#ifndef FILE
#define FILE u8_t
#endif // FILE

int fprintf(FILE *restrict stream, const char *restrict fmt, ...);
int vfprintf(FILE *restrict stream, const char *restrict fmt, va_list ap);

int dprintf(int fd, const char *restrict fmt, ...);
int vdprintf(int fd, const char *restrict fmt, va_list ap);

int sprintf(char *restrict str, const char *restrict fmt, ...);
int vsprintf(char *restrict str, const char *restrict fmt, va_list ap);

int snprintf(char *restrict str, size_t size, const char *restrict fmt, ...);
int vsnprintf(char *restrict str, size_t size, const char *restrict fmt, va_list ap);

#endif // STDIO_H