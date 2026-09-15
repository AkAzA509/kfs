#include <stdio.h>
#include <unistd.h>

int fputc(int c, FILE *stream)
{
	if (!stream)
		return -1;

	if (stream->mode == _IONBF) {
		char ch = (char)c;
		write(stream->fd, &ch, 1);
		return c;
	}

	stream->buffer[stream->buf_pos++] = (char)c;

	if ((stream->mode == _IOLBF && (char)c == '\n') ||
	    (stream->buf_pos >= BUFSIZ))
		fflush(stream);

	return c;
}
