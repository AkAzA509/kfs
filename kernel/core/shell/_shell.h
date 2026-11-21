#ifndef _SHELL_H
#define _SHELL_H

#define CMD_BUF_SIZE 256
#define MAX_ARGS 20

typedef int (*cmd_fn)(int argc, char **argv);

struct command {
	const char *name;
	cmd_fn fn;
};

int tokenize(char *line, char **argv, int max_args);

#endif // _SHELL_H
