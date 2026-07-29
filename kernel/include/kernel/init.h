#ifndef INIT_H
#define INIT_H

#include <stdbool.h>

void	debug_screen(void);
void	debug_font(void);
void	debug_current_screen(void);

typedef struct t_multiboot_info multiboot_info;

void	init_gdt(void);
bool	init_term(void);
void	init_display(multiboot_info *mbi);

#endif // INIT_H