#ifndef TEST_COMMON_H
#define TEST_COMMON_H

#include "../../libc/include/string.h"

long write(int fd, const void *buf, size_t count);
const char *stub_get_written_data(void);
void stub_clear(void);
void real_write(const char *str, size_t len);

#endif // TEST_COMMON_H
