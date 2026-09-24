#include <string.h>
#include <ctype.h>

char *trim(char *s)
{
	char *end;

	if (/*!s ||*/ !*s)
		return s;

	while (isspace((unsigned char)*s))
		s++;

	end = s + strlen(s) - 1;
	while (end >= s && isspace((unsigned char)*end))
		end--;

	*(end + 1) = '\0';
	return s;
}