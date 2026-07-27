#ifndef TTY_H
#define TTY_H

#include <stdint.h>

// command list for the tty
// - reboot
// - halt
// - print logo
// - print stack
// - shutdown
// - (change color)

void	editor_backspace(void);
void	editor_delete(void);
void	editor_putchar(char c);
void	editor_start(void);
void	move_cursor(i8_t direction);
// void	line_editor_submit(void);

#endif // TTY_H