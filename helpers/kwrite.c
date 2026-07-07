#include "../includes/stddef.h"
#include "helpers.h"

void	kwrite(const void* data, size_t size)
{
	const char	*str = data;
	int	tmp = 0;

	for (size_t i = 0; i < size; i++)
		kputchar(str[i]);
}