#ifndef STDARG_H
#define STDARG_H

// typedef char* va_list;

// // aligne the memory to access the right next element
// #define __va_align(size)	(((size) + sizeof(int) - 1) & ~(sizeof(int) - 1))

// // // take the addresse cast into a 1 byte ptr of the last args and jump after him
// #define va_start(ap, last)	(ap = (char *)&(last) + __va_align(sizeof(last)))

// // // take the lenght of the ptr (type) jump after him and return is start
// #define va_arg(ap, type)	(*(type *)((ap += __va_align(sizeof(type))) - __va_align(sizeof(type))))

// // copy the va_list
// #define va_copy(dest, src)	((dest) = (src))

// // // clean the ptr
// #define va_end(ap)			(ap = (void *)0)

typedef __builtin_va_list va_list;

#define va_start(ap, last) __builtin_va_start(ap, last)
#define va_arg(ap, type) __builtin_va_arg(ap, type)
#define va_end(ap) __builtin_va_end(ap)
#define va_copy(dest, src) __builtin_va_copy(dest, src)

#endif // STDARG_H