#include <ctype.h>

[[nodiscard]] int isprint(int c)
{
	return (c >= 32 && c <= 126);
}