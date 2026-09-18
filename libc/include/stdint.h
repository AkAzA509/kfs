#ifndef STDINT_H
#define STDINT_H

typedef unsigned char u8_t;
typedef unsigned short u16_t;
typedef unsigned int u32_t;
typedef unsigned long long u64_t;

typedef signed char i8_t;
typedef signed short i16_t;
typedef signed int i32_t;

typedef __UINTPTR_TYPE__ uintptr_t;
typedef __INTPTR_TYPE__ intptr_t;

static_assert(sizeof(u8_t) == 1);
static_assert(sizeof(u16_t) == 2);
static_assert(sizeof(u32_t) == 4);
static_assert(sizeof(u64_t) == 8);
static_assert(sizeof(uintptr_t) == sizeof(void *));

#endif // STDINT_H