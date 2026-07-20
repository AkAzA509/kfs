#ifndef STDIO_H
#define STDIO_H

#include <stdint.h>
#include <stddef.h>
#include <stdarg.h>

int printf(const char *restrict format, ...);
int vprintf(const char *restrict format, va_list ap);

#ifndef FILE
#define FILE u8_t
#endif // FILE

int fprintf(FILE *restrict stream, const char *restrict format, ...);
int vfprintf(FILE *restrict stream, const char *restrict format, va_list ap);

int dprintf(int fd, const char *restrict format, ...);
int vdprintf(int fd, const char *restrict format, va_list ap);

int sprintf(char *restrict str, const char *restrict format, ...);
int vsprintf(char *restrict str, const char *restrict format, va_list ap);

int snprintf(char str[restrict .size], size_t size, const char *restrict format, ...);
int vsnprintf(char str[restrict .size], size_t size, const char *restrict format, va_list ap);

#endif // STDIO_H