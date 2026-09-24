#include "../libc/include/ctype.h"
#include "include/test_common.h"

static int ctype_pass = 0;
static int ctype_fail = 0;

static void check_is_func(const char *orig, bool ret, bool expected)
{
	if (ret == expected) {
		ctype_pass++;
		t_print("[OK]   ");
		t_print(orig);
		t_print("\n");
	} else {
		ctype_fail++;
		t_print("[FAIL] ");
		t_print(orig);
		t_print("  expected=");
		t_print_int(expected);
		t_print(" res=");
		t_print_int(ret);
		t_print("\n");
	}
}

static void run_isalnum()
{
	print_func_name("isalnum");

	char c = 'c';
	check_is_func(&c, isalnum(c), true);

	c = 'W';
	check_is_func(&c, isalnum(c), true);

	c = '\t';
	check_is_func(&c, isalnum(c), false);

	c = '1';
	check_is_func(&c, isalnum(c), true);

	c = 'Z';
	check_is_func(&c, isalnum(c), true);

	c = 123;
	check_is_func(&c, isalnum(c), false);

	c = 0;
	check_is_func(&c, isalnum(c), false);

	c = '7';
	check_is_func(&c, isalnum(c), true);

	c = 124;
	check_is_func(&c, isalnum(c), false);

	c = '\n';
	check_is_func(&c, isalnum(c), false);

	c = -1;
	check_is_func(&c, isalnum(c), false);

	c = -50;
	check_is_func(&c, isalnum(c), false);
}

static void run_isalpha()
{
	print_func_name("isalpha");

	char c = 'c';
	check_is_func(&c, isalpha(c), true);

	c = 'W';
	check_is_func(&c, isalpha(c), true);

	c = '\t';
	check_is_func(&c, isalpha(c), false);

	c = 12;
	check_is_func(&c, isalpha(c), false);

	c = 'Z';
	check_is_func(&c, isalpha(c), true);

	c = 123;
	check_is_func(&c, isalpha(c), false);

	c = 0;
	check_is_func(&c, isalpha(c), false);

	c = ' ';
	check_is_func(&c, isalpha(c), false);

	c = 124;
	check_is_func(&c, isalpha(c), false);

	c = '\n';
	check_is_func(&c, isalpha(c), false);

	c = -1;
	check_is_func(&c, isalpha(c), false);

	c = -50;
	check_is_func(&c, isalpha(c), false);
}

static void run_isdigit()
{
	print_func_name("isdigit");

	char c = 'c';
	check_is_func(&c, isdigit(c), false);

	c = '5';
	check_is_func(&c, isdigit(c), true);

	c = '\t';
	check_is_func(&c, isdigit(c), false);

	c = 12;
	check_is_func(&c, isdigit(c), false);

	c = 'Z';
	check_is_func(&c, isdigit(c), false);

	c = 123;
	check_is_func(&c, isdigit(c), false);

	c = 0;
	check_is_func(&c, isdigit(c), false);

	c = '9';
	check_is_func(&c, isdigit(c), true);

	c = 124;
	check_is_func(&c, isdigit(c), false);

	c = '0';
	check_is_func(&c, isdigit(c), true);

	c = -1;
	check_is_func(&c, isdigit(c), false);

	c = -50;
	check_is_func(&c, isdigit(c), false);
}

static void run_islower()
{
	print_func_name("islower");

	char c = 'c';
	check_is_func(&c, islower(c), true);

	c = 'W';
	check_is_func(&c, islower(c), false);

	c = '\t';
	check_is_func(&c, islower(c), false);

	c = 12;
	check_is_func(&c, islower(c), false);

	c = 'Z';
	check_is_func(&c, islower(c), false);

	c = 123;
	check_is_func(&c, islower(c), false);

	c = 'p';
	check_is_func(&c, islower(c), true);

	c = ' ';
	check_is_func(&c, islower(c), false);

	c = 124;
	check_is_func(&c, islower(c), false);

	c = '\n';
	check_is_func(&c, islower(c), false);

	c = -1;
	check_is_func(&c, islower(c), false);

	c = -50;
	check_is_func(&c, islower(c), false);
}

static void run_isprint()
{
	print_func_name("isprint");

	char c = 'c';
	check_is_func(&c, isprint(c), true);

	c = 'W';
	check_is_func(&c, isprint(c), true);

	c = '\t';
	check_is_func(&c, isprint(c), false);

	c = 12;
	check_is_func(&c, isprint(c), false);

	c = 'Z';
	check_is_func(&c, isprint(c), true);

	c = 123;
	check_is_func(&c, isprint(c), true);

	c = 0;
	check_is_func(&c, isprint(c), false);

	c = ' ';
	check_is_func(&c, isprint(c), true);

	c = 124;
	check_is_func(&c, isprint(c), true);

	c = '\n';
	check_is_func(&c, isprint(c), false);

	c = -1;
	check_is_func(&c, isprint(c), false);

	c = -50;
	check_is_func(&c, isprint(c), false);
}

static void run_isspace()
{
	print_func_name("isspace");

	char c = 'c';
	check_is_func(&c, isspace(c), false);

	c = 'W';
	check_is_func(&c, isspace(c), false);

	c = '\t';
	check_is_func(&c, isspace(c), true);

	c = 12;
	check_is_func(&c, isspace(c), true);

	c = 'Z';
	check_is_func(&c, isspace(c), false);

	c = 123;
	check_is_func(&c, isspace(c), false);

	c = 0;
	check_is_func(&c, isspace(c), false);

	c = ' ';
	check_is_func(&c, isspace(c), true);

	c = 124;
	check_is_func(&c, isspace(c), false);

	c = '\n';
	check_is_func(&c, isspace(c), true);

	c = -1;
	check_is_func(&c, isspace(c), false);

	c = -50;
	check_is_func(&c, isspace(c), false);
}

static void run_isupper()
{
	print_func_name("isupper");

	char c = 'c';
	check_is_func(&c, isupper(c), false);

	c = 'W';
	check_is_func(&c, isupper(c), true);

	c = '\t';
	check_is_func(&c, isupper(c), false);

	c = 12;
	check_is_func(&c, isupper(c), false);

	c = 'Z';
	check_is_func(&c, isupper(c), true);

	c = 123;
	check_is_func(&c, isupper(c), false);

	c = 0;
	check_is_func(&c, isupper(c), false);

	c = 'R';
	check_is_func(&c, isupper(c), true);

	c = 124;
	check_is_func(&c, isupper(c), false);

	c = 127;
	check_is_func(&c, isupper(c), false);

	c = -1;
	check_is_func(&c, isupper(c), false);

	c = -50;
	check_is_func(&c, isupper(c), false);
}

static void run_isascii()
{
	print_func_name("isascii");

	char c = 'c';
	check_is_func(&c, isascii(c), true);

	c = 'W';
	check_is_func(&c, isascii(c), true);

	c = '\t';
	check_is_func(&c, isascii(c), true);

	c = 12;
	check_is_func(&c, isascii(c), true);

	c = '}';
	check_is_func(&c, isascii(c), true);

	c = 145;
	check_is_func(&c, isascii(c), false);

	c = 0;
	check_is_func(&c, isascii(c), true);

	c = 'R';
	check_is_func(&c, isascii(c), true);

	c = 124;
	check_is_func(&c, isascii(c), true);

	c = 127;
	check_is_func(&c, isascii(c), true);

	c = -1;
	check_is_func(&c, isascii(c), false);

	c = -50;
	check_is_func(&c, isascii(c), false);
}

void test_ctype()
{
	ctype_pass = 0;
	ctype_fail = 0;

	print_suite_name("IS FUNC");

	run_isalnum();
	run_isalpha();
	run_isdigit();
	run_islower();
	run_isprint();
	run_isspace();
	run_isupper();
	run_isascii();
}

void get_ctype_score(int *pass, int *fail, int *total)
{
	*pass = ctype_pass;
	*fail = ctype_fail;
	*total = ctype_pass + ctype_fail;
}