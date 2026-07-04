#include "../includes/stddef.h"
#include "../kernel/terminal.h"
#include "helpers.h"

void	kwrite(const void* data, size_t size)
{
	int tmp = 0;
	const char *str = data;

	for (size_t i = 0; i < size; i++) {
		if (tmp >= 15)
			tmp = 0;
		kputchar(str[i]);
		tmp++;
	}
	flush();
}