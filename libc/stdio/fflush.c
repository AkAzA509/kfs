#include <stdio.h>
#include <unistd.h>

[[gnu::nonnull(1)]]
int fflush(FILE *stream)
{
	// if (!stream)
	// 	return -1;

	if (stream->buf_pos > 0) {
		write(stream->fd, stream->buffer, stream->buf_pos);
		stream->buf_pos = 0;
	}
	return 0;
}
