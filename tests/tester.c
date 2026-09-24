#include "include/test_common.h"

int g_pass = 0;
int g_fail = 0;

static void print_suite_summarize(const char *name,
				  void (*f)(int *, int *, int *), int *all_pass,
				  int *all_fail)
{
	int fail = 0, pass = 0, total = 0;

	t_print(" - ");
	t_print(name);
	t_print(":\t ");
	f(&pass, &fail, &total);
	*all_pass += pass;
	*all_fail += fail;
	if (fail != 0) {
		t_print(RED);
		t_print_int(fail);
		t_print(" / ");
		t_print_int(total);
		t_print("  ");
	}
	if (pass != 0) {
		t_print(GREEN);
		t_print_int(pass);
		t_print(" / ");
		t_print_int(total);
	}
	t_print(RESET);
	t_print("\n");
}

static int print_resume(void)
{
	int total_pass = 0;
	int total_fail = 0;

	t_print("\n========================================\n");
	t_print("Finale summarize: \n");
	print_suite_summarize("string", get_string_score, &total_pass,
			      &total_fail);
	print_suite_summarize("ctype", get_ctype_score, &total_pass,
			      &total_fail);
	print_suite_summarize("stdio", get_stdio_score, &total_pass,
			      &total_fail);
	print_suite_summarize("stdlib", get_stdlib_score, &total_pass,
			      &total_fail);
	t_print("\nTotal: ");
	t_print_int(total_pass);
	t_print(" passed, ");
	t_print_int(total_fail);
	t_print(" failed\n");
	t_print("\n========================================\n" RESET);

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
