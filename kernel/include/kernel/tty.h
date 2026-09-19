#ifndef TTY_H
#define TTY_H

typedef enum {
	move_one_left = -1,
	move_one_right = 1,
	move_start = 0,
	move_end = 2
} e_editor_move;

void editor_backspace(void);
void editor_delete(void);
void editor_putchar(char c);
void editor_start(void);
void editor_switch(int new_id);
void move_cursor(e_editor_move editor_move);

#ifdef DEBUG
void debug_editor(void);
#endif //DEBUG

#endif // TTY_H