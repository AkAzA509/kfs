#include <ctype.h>

[[nodiscard]] int isdigit(int c)
{
	return (c >= '0' && c <= '9');
}