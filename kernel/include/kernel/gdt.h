#ifndef GDT_H
#define GDT_H

#include <stdint.h>

typedef struct s_gdt {
	u8_t	entry;
	u8_t	base;
	u8_t	limit;
	u8_t	access;
	u8_t	flags;
}			t_gdt;

static t_gdt gdt[10];

struct t_lgdt {
	u8_t	base;
	u32_t	addr;
};

#endif // GDT_H