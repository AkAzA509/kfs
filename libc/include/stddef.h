#ifndef STDDEF_H
#define STDDEF_H

typedef __SIZE_TYPE__ size_t;
typedef __PTRDIFF_TYPE__ ssize_t;

#define NULL ((void *)0)

static_assert(sizeof(size_t) == sizeof(void *));
static_assert(sizeof(ssize_t) == sizeof(size_t));

#endif // STDDEF_H
