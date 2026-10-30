#include <stdlib.h>

long labs(long x)
{
	return x < 0 ? -x : x;
}

int abs(int x)
{
	return x < 0 ? -x : x;
}

unsigned long long ullabs(long long x)
{
	return x < 0 ? -x : x;
}