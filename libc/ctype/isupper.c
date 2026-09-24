#include <ctype.h>

[[nodiscard]] int isupper(int c)
{
	return (c >= 'A' && c <= 'Z');
}