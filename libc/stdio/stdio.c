#include <stdio.h>

static FILE _stdin_stream = { .fd = 0, .buf_pos = 0, .mode = _IONBF };
static FILE _stdout_stream = { .fd = 1, .buf_pos = 0, .mode = _IOLBF };
static FILE _stderr_stream = { .fd = 2, .buf_pos = 0, .mode = _IOLBF };
static FILE _vterm1_stream = { .fd = 3, .buf_pos = 0, .mode = _IOLBF };
static FILE _vterm2_stream = { .fd = 4, .buf_pos = 0, .mode = _IOLBF };
static FILE _vterm3_stream = { .fd = 5, .buf_pos = 0, .mode = _IOLBF };
static FILE _vterm4_stream = { .fd = 6, .buf_pos = 0, .mode = _IOLBF };

FILE *stdin = &_stdin_stream;
FILE *stdout = &_stdout_stream;
FILE *stderr = &_stderr_stream;
FILE *vterm1 = &_vterm1_stream;
FILE *vterm2 = &_vterm2_stream;
FILE *vterm3 = &_vterm3_stream;
FILE *vterm4 = &_vterm4_stream;