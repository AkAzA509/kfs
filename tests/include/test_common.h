#ifndef TEST_COMMON_H
#define TEST_COMMON_H

// #include "../../libc/include/string.h"
#include "../../libc/include/stddef.h"

#define GREEN "\033[92m"
#define WHITE "\033[97m"
#define BLUE "\033[94m"
#define RED "\033[91m"
#define RESET "\033[0m"

long write(int fd, const void *buf, size_t count);
const char *stub_get_written_data(void);
void stub_clear(void);
void real_write(const char *str, size_t len);

void print_result(int pass, int fail);
void print_suite_name(const char *suite);
void print_func_name(const char *func);
void t_print(const char *s);
void t_print_int(int n);

#endif // TEST_COMMON_H
