#include <stdio.h>
#include <unistd.h>

[[gnu::nonnull(2)]]
int fputc(int c, FILE *stream)
{
	if (stream->mode == _IONBF) {
		char ch = (char)c;
		if (write(stream->fd, &ch, 1) != 1)
			return -1;
		return c;
	}

	if (stream->buf_pos >= BUFSIZE && fflush(stream) < 0)
		return -1;

	stream->buffer[stream->buf_pos++] = (char)c;

	if ((stream->mode == _IOLBF && (char)c == '\n') ||
	    (stream->buf_pos >= BUFSIZE))
		if (fflush(stream) < 0)
			return -1;

	return c;
}
