#include <stdio.h>
#include <unistd.h>

[[gnu::nonnull(1)]]
int fflush(FILE *stream)
{
	if (stream->buf_pos > 0) {
		ssize_t written =
			write(stream->fd, stream->buffer, stream->buf_pos);
		if (written < 0 || (size_t)written != stream->buf_pos)
			return -1;
		stream->buf_pos = 0;
	}
	return 0;
}
