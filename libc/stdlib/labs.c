#include <stdlib.h>

long labs(long x) [[reproducible]]
{
	return x < 0 ? -x : x;
}

int abs(int x) [[reproducible]]
{
	return x < 0 ? -x : x;
}

unsigned long long ullabs(long long x) [[reproducible]]
{
	return x < 0 ? -x : x;
}