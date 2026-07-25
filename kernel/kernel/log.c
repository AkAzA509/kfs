#include <kernel/log.h>
#include <kernel/kernel.h>
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

	void serial_print_hex(u32_t val)
{
	char hex[] = "0123456789abcdef";
	outb(0x3F8, '0');
	outb(0x3F8, 'x');
	for (int i = 7; i >= 0; i--)
		outb(0x3F8, hex[(val >> (i * 4)) & 0xF]);
	outb(0x3F8, '\n');
}

void serial_print(char val)
{
	outb(0x3F8, val);
}

static void	_vprint_e(void *ctx, char c)
{
	(void)ctx;
	serial_print(c);
}

#include "../../libc/stdio/vprint_core.h"
static int	kvprintf(const char *restrict fmt, va_list ap)
{
	out_target_t target = {
		.emit = _vprint_e,
		.ctx = NULL,
		.count = 0
	};

	int ret = vprint_core(fmt, &ap, &target);

	return ret;
}

int	klog(const char *restrict fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	int ret = kvprintf(fmt, ap);
	va_end(ap);
	return ret;
}