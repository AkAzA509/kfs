#include <kernel/log.h>
#include <stdint.h>
#include <stddef.h>
#include <stdio.h>

static inline	u32_t get_esp(void)
{
	u32_t	esp;
	__asm__ volatile("mov %%esp, %0" : "=r"(esp));
	return esp;
}

extern u32_t stack_top;

void	log_stack(size_t len)
{
	u32_t esp = get_esp();
	u32_t top = (u32_t)&stack_top;
	size_t nb_words = len > 0 ? len : (top - esp) / 4;

	printf("kernel stack log from start to end\n(higher address mean near from start)\n");
	printf(" addresse :   value\n");
	// u32_t *stack = (u32_t *)top;
	for (size_t i = 0; i != nb_words; i++) {
		u32_t addr = top - (i * 4) - 4;
		printf("0x%08x: 0x%08x\n", addr, *(u32_t *)addr);
	}
}