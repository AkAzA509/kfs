#include "stddef.h"
#include <string.h>
#include <stdio.h>

size_t kfs_strlen(const char *str);

static int g_pass = 0;
static int g_fail = 0;

static void check_results(const char *s, size_t ret_libc, size_t ret_kfs)
{
	if (ret_libc == ret_kfs) {
		g_pass++;
		printf("[OK]   ");
		printf("%s", s);
		printf("\n");
	} else {
		g_fail++;
		printf("[FAIL] ");
		printf("%s", s);
		printf("  libc_ret=");
		printf("%zu", ret_libc);
		printf(" kfs_ret=");
		printf("%zu", ret_kfs);
		printf("\"\n");
	}
}

typedef void (*test_runner_t)(const char *s);

void run_strlen(const char *s)
{
	size_t ret1 = strlen(s);
	size_t ret2 = kfs_strlen(s);

	check_results(s, ret1, ret2);
}


void string_suite(test_runner_t run, const char *suite_name)
{
	printf("\n========================================\n");
	printf(" RUNNING SUITE: %s", suite_name);
	printf("\n========================================\n");

	run("Hello world!");
	// run(NULL);
	run("\n");
	run("                                   ");
	run("0000000000000000000000000000000000000000000000000000000000000000000000000000");
	run("");
	run("\tev\34\42\23\34\5h\65\tg\edcsdf\g\g\dfsdcdcsd");
}

// --- Blobal tester ---
// void test_mem()
// {
	
// }

#define GREEN "\033[92m"
#define WHITE "\033[97m"
#define BLUE "\033[94m"
#define RED "\033[91m"
#define RESET "\033[0m"

void test_str()
{
	printf(BLUE"========================================\n");
	printf(" STRING TEST SUITE\n");
	printf("========================================\n"RESET);
	
	string_suite(run_strlen, "strlen");

	printf(g_fail > 0 ? RED : GREEN);
	printf("\n========================================\n");
	printf("TOTAL PASS: %d", g_pass);
	printf("   TOTAL FAIL: %d", g_fail);
	printf("\n========================================\n"RESET);
}

int main()
{
	g_pass = 0;
	g_fail = 0;

	// test_mem();
	test_str();

	return g_fail != 0;
}