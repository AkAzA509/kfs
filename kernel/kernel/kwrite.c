#include <kernel/kprint/kprint.h>
#include <stddef.h>

void	kwrite(const void* data, size_t size)
{
	const char	*str = data;

	for (size_t i = 0; i < size; i++)
		kputchar(str[i]);
}