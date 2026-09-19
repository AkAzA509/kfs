#include <stdio.h>

static FILE _stdin_stream = { .fd = 0, .buf_pos = 0, .mode = _IONBF };
static FILE _stdout_stream = { .fd = 1, .buf_pos = 0, .mode = _IOLBF };
static FILE _stderr_stream = { .fd = 2, .buf_pos = 0, .mode = _IOLBF };

FILE *stdin = &_stdin_stream;
FILE *stdout = &_stdout_stream;
FILE *stderr = &_stderr_stream;