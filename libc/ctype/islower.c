#include <ctype.h>

[[nodiscard]] int islower(int c)
{
	return (c >= 'a' && c <= 'z');
}