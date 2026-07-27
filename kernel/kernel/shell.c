#include <kernel/common.h>
#include <kernel/shell.h>
#include <kernel/log.h>
#include <string.h>
#include <stdio.h>

static const char *cmd_table[] = {
	"reboot", "halt", "print logo", "print stack", "shutdown", NULL
};

static void	cmd_reboot(void) { REBOOT }
static void	cmd_halt(void) { __asm__ volatile ("cli"); HALT_ERROR; }
static void	cmd_print_logo(void) { ASCII_LOGO; }
static void	cmd_print_stack(void){ log_stack(0); }
static void	cmd_shutdown(void) { SHUTDOWN; }

typedef void (*cmd_handler_t)(void);
static const cmd_handler_t cmd_handlers[] = {
	cmd_reboot, cmd_halt, cmd_print_logo, cmd_print_stack, cmd_shutdown
};

void	shell_execute(const char *input, size_t len)
{
	for (u16_t i = 0; cmd_table[i]; i++) {
		if (strncmp(input, cmd_table[i], len) == 0 && cmd_table[i][len] == '\0') {
			cmd_handlers[i]();
			return ;
		}
	}
	// printf("commande inconnue: %.*s\n", (int)len, input);
}

void	print_prompt(void)
{
	printf("Tekos/root > ");
}