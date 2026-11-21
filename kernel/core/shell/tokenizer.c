
int tokenize(char *line, char **argv, int max_args)
{
	int argc = 0;

	while (*line) {
		while (*line == ' ' || *line == '\t')
			*line++ = '\0'; // saute les espaces et les coupe
		if (!*line)
			break;
		if (argc < max_args - 1) // garde la place pour le NULL final
			argv[argc++] = line;
		while (*line && *line != ' ' && *line != '\t')
			line++; // avance jusqu'à la fin du mot
	}
	argv[argc] = 0;
	return argc;
}
