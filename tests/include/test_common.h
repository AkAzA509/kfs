#ifndef TEST_COMMON_H
#define TEST_COMMON_H

#include <stdbool.h>
#include <string.h>
#include <stdio.h>

#define assert(expected, result, line, function)                                          \
	{                                                                                 \
		if (strcmp(expected, result)) {                                           \
			fprintf(stderr,                                                   \
				"assertion failed in %s on line %s: %s differ from %s\n", \
				function, line, reslut, expected);                        \
			return false;                                                     \
		}                                                                         \
		return true;                                                              \
	}

#endif // TEST_COMMON_H
