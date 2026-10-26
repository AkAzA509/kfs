#include "include/test_common.h"

int g_pass = 0;
int g_fail = 0;

static void print_suite_summarize(const char *name,
				  void (*f)(int *, int *, int *), int *all_pass,
				  int *all_fail)
{
	int fail = 0, pass = 0, total = 0;

	real_printf(" - %s:\t ", name);

	f(&pass, &fail, &total);
	*all_pass += pass;
	*all_fail += fail;
	if (fail != 0)
		real_printf(RED "%d / %d  ", fail, total);
	if (pass != 0)
		real_printf(GREEN "%d / %d", pass, total);
	real_printf("\n" RESET);
}

static int print_resume(void)
{
	int total_pass = 0;
	int total_fail = 0;

	real_printf("\n ========================================\n");
	real_printf(" Finale summarize: \n");
	print_suite_summarize("string", get_string_score, &total_pass,
			      &total_fail);
	print_suite_summarize("ctype", get_ctype_score, &total_pass,
			      &total_fail);
	print_suite_summarize("stdio", get_stdio_score, &total_pass,
			      &total_fail);
	print_suite_summarize("stdlib", get_stdlib_score, &total_pass,
			      &total_fail);
	real_printf("\n Total: %d passed, %d failed\n", total_pass, total_fail);
	real_printf("\n ========================================\n" RESET);

	return total_fail != 0;
}

int main()
{
	test_string();
	test_ctype();
	test_stdio();
	test_stdlib();

	return print_resume();
}
