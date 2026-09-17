#ifndef STDDEF_H
#define STDDEF_H

typedef unsigned long size_t;
typedef signed int ssize_t;

#define NULL ((void *)0)

static_assert(sizeof(size_t) == sizeof(void *));
static_assert(sizeof(ssize_t) == sizeof(size_t));

#endif // STDDEF_H