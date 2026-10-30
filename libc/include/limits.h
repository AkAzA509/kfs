#ifndef LIMITS_H
#define LIMITS_H

#define INT_MAX __INT_MAX__
#define INT_MIN (-__INT_MAX__ - 1)

#define LONG_MAX __LONG_MAX__
#define LONG_MIN (-__LONG_MAX__ - 1L)
#define ULONG_MAX (LONG_MAX * 2UL + 1UL)

#define LLONG_MAX __LONG_LONG_MAX__
#define LLONG_MIN (-__LONG_LONG_MAX__ - 1LL)
#define ULLONG_MAX (LLONG_MAX * 2ULL + 1ULL)

#endif // LIMITS_H
