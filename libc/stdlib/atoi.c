#include <ctype.h>
#include <limits.h>

int atoi(const char *str)
{
	int sign = 1;
	int digit;
	unsigned int result = 0;
	unsigned int limit;

	while (isspace(*str))
		str++;
	if (*str == '-' || *str == '+') {
		if (*str == '-')
			sign = -1;
		str++;
	}
	limit = sign == -1 ? -(unsigned int)INT_MIN : INT_MAX;
	for (; isdigit(*str); str++) {
		digit = *str - '0';
		if (result > (limit - digit) / 10)
			return sign == -1 ? INT_MIN : INT_MAX;
		result = result * 10 + digit;
	}

	return (int)(sign * result);
}