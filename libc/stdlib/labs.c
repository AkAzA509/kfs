
long labs(long x) [[reproducible]]
{
	return x < 0 ? -x : x;
}

int abs(int x) [[reproducible]]
{
	return x < 0 ? -x : x;
}
