#ifndef SHELL_H
#define SHELL_H

#include <stddef.h>

void	shell_execute(const char *input, size_t len);
void	print_prompt(void);

#endif // SHELL_H