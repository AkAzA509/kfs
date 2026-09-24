#include <ctype.h>

[[nodiscard]] int isascii(int c)
{
	return (c >= 0 && c <= 127);
}