#include <arch/i386/gdt.h>
#include <core/common.h>
#include <core/init.h>
#include <core/io.h>
#include <core/log.h>
#include <core/shell.h>
#include <drivers/console.h>
#include <drivers/keyboard.h>

#include <_attrs.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

#include "_cmds.h"
#include "_shell.h"

// typedef void (*cmd_handler_t)(void);
// static const cmd_handler_t cmd_handlers[] = {
// 	cmd_reboot,    cmd_halt,	 cmd_print_logo, cmd_print_stack,
// 	cmd_shutdown,  cmd_date,	 cmd_help,	 cmd_clear,
// 	cmd_print_gdt, cmd_print_screen, cmd_change_lay,
// };

// void shell_execute(char *input)
// {
// 	const char *trim_input = trim(input);
// 	size_t len = strlen(trim_input);
// 	if (len <= 0)
// 		return;
// 	for (u16_t i = 0; cmd_table[i]; i++) {
// 		if (strncmp(trim_input, cmd_table[i], len) == 0 &&
// 		    strlen(cmd_table[i]) == len) {
// 			cmd_handlers[i]();
// 			return;
// 		}
// 	}
// 	printf("commande not found: %.*s\n", (int)len, trim_input);
// }

void print_prompt(void)
{
	printf("Tekos/kernel > ");
}

static const struct command commands[] = {
	{ "echo", echo },	  { "help", help },	  { "reboot", reboot },
	{ "halt", halt },	  { "plogo", plogo },	  { "pstack", pstack },
	{ "shutdown", shutdown }, { "date", date },	  { "clear", clear },
	{ "pgdt", pgdt },	  { "pscreen", pscreen }, { "layout", layout },
};

static char *argv[MAX_ARGS];

void shell_execute(char *line)
{
	int argc = tokenize(line, argv, MAX_ARGS);
	if (argc == 0)
		return;
	for (size_t i = 0; i < sizeof(commands) / sizeof(*commands); i++) {
		if (!strcmp(argv[0], commands[i].name)) {
			commands[i].fn(argc, argv);
			return;
		}
	}
	printf("unknown command: %s\n", argv[0]);
}
