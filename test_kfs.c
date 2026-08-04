#include "stddef.h"
#include <stdint.h>
#include <stdio.h>

#define PINK "\033[38;5;206m"

void test(int t)
{
	int col = 12;
	printf("col += %d = %d\n", t, col += t);
}

int main()
{
	// u16_t test =(0x04 << 8) | 0x63;

	// printf(PINK"     ███        ▄████████    ▄█   ▄█▄  ▄██████▄     ▄████████\n");
	// printf(" ▀█████████▄   ███    ███   ███ ▄███▀ ███    ███   ███    ███\n");
	// printf("    ▀███▀▀██   ███    █▀    ███▐██▀   ███    ███   ███    █▀ \n");
	// printf("     ███   ▀  ▄███▄▄▄      ▄█████▀    ███    ███   ███       \n");
	// printf("     ███     ▀▀███▀▀▀     ▀▀█████▄    ███    ███ ▀███████████\n");
	// printf("     ███       ███    █▄    ███▐██▄   ███    ███          ███\n");
	// printf("     ███       ███    ███   ███ ▀███▄ ███    ███    ▄█    ███\n");
	// printf("    ▄████▀     ██████████   ███   ▀█▀  ▀██████▀   ▄████████▀ \n");
	// printf("                            ▀                                \033[0m\n");

	// u32_t bg = 0xFFFFFF;
	// u32_t cursor = 0xFFFFFF;

	// u8_t col = 15 | (0 << 4);

	// u8_t retrieve_bg = col >> 4;
	// // printf("%x, %x\n", bg, cursor);
	// // printf("%x", ~cursor);
	// printf("%x\n", col);
	// printf("%x\n", retrieve_bg);

	// char *s = NULL;
	// printf("test flag %s\n", s);

	// printf("%zu\n", sizeof(size_t));
	// long test = -12345L;
	// printf("val: %ld, abs: %ld\n", test, ~test);

	test(1);
	test(-1);
}

// #include <stdio.h>
// #include <limits.h>
// #include <stdint.h>

// #define T_INT(fmt, val) \
// 	printf("\t{ \"%s\", \"", fmt); \
// 	printf(fmt, val); \
// 	printf("\" },\n")

// #define T_UINT(fmt, val) \
// 	printf("\t{ \"%s\", \"", fmt); \
// 	printf(fmt, val); \
// 	printf("\" },\n")

// #define T_STR(fmt, val) \
// 	printf("\t{ \"%s\", \"", fmt); \
// 	printf(fmt, val); \
// 	printf("\" },\n")

// #define T_CHR(fmt, val) \
// 	printf("\t{ \"%s\", \"", fmt); \
// 	printf(fmt, val); \
// 	printf("\" },\n")

// #define T_FLT(fmt, val) \
// 	printf("\t{ \"%s\", \"", fmt); \
// 	printf(fmt, val); \
// 	printf("\" },\n")

// #define T_PTR(fmt, val) \
// 	printf("\t{ \"%s\", \"", fmt); \
// 	printf(fmt, val); \
// 	printf("\" },\n")

// int main(void)
// {
// 	printf("// ===== %%d / %%i =====\n");
// 	T_INT("%d", 0);
// 	T_INT("%d", 42);
// 	T_INT("%d", -42);
// 	T_INT("%i", 123);
// 	T_INT("%5d", 42);
// 	T_INT("%-5d", 42);
// 	T_INT("%05d", 42);
// 	T_INT("%05d", -42);
// 	T_INT("%+d", 42);
// 	T_INT("%+d", -42);
// 	T_INT("% d", 42);
// 	T_INT("%.5d", 42);
// 	T_INT("%.0d", 0);
// 	T_INT("%8.5d", 42);
// 	T_INT("%-8.5d", 42);
// 	T_INT("%08.5d", 42);   // precision présente -> flag 0 ignoré
// 	T_INT("%lld", 9223372036854775807LL);
// 	T_INT("%lld", LLONG_MIN);
// 	T_INT("%hhd", (signed char)200);   // wrap
// 	T_INT("%hd", (short)70000);        // wrap
// 	T_INT("%ld", 123456789L);
// 	T_INT("%d", INT_MIN);
// 	T_INT("%d", INT_MAX);

// 	printf("\n// ===== %%u =====\n");
// 	T_UINT("%u", 0u);
// 	T_UINT("%u", 42u);
// 	T_UINT("%u", UINT_MAX);
// 	T_UINT("%5u", 42u);
// 	T_UINT("%-5u", 42u);
// 	T_UINT("%05u", 42u);
// 	T_UINT("%llu", 18446744073709551615ULL);

// 	printf("\n// ===== %%x / %%X =====\n");
// 	T_UINT("%x", 255);
// 	T_UINT("%X", 255);
// 	T_UINT("%#x", 255);
// 	T_UINT("%#X", 255);
// 	T_UINT("%#x", 0);
// 	T_UINT("%08x", 255);
// 	T_UINT("%#08x", 255);
// 	T_UINT("%8x", 255);
// 	T_UINT("%-8x", 255);
// 	T_UINT("%.8x", 255);
// 	T_UINT("%x", 0xDEADBEEF);
// 	T_UINT("%llx", 0xFFFFFFFFFFFFFFFFULL);

// 	printf("\n// ===== %%o =====\n");
// 	T_UINT("%o", 8);
// 	T_UINT("%o", 0);
// 	T_UINT("%#o", 8);
// 	T_UINT("%#o", 0);
// 	T_UINT("%#o", 511);
// 	T_UINT("%6o", 8);

// 	printf("\n// ===== %%s =====\n");
// 	T_STR("%s", "hello");
// 	T_STR("%10s", "hi");
// 	T_STR("%-10s", "hi");
// 	T_STR("%.3s", "bonjour");
// 	T_STR("%10.3s", "bonjour");
// 	T_STR("%-10.3s", "bonjour");
// 	T_STR("%s", "");
// 	T_STR("%.0s", "abc");

// 	printf("\n// ===== %%c =====\n");
// 	T_CHR("%c", 'a');
// 	T_CHR("%5c", 'a');
// 	T_CHR("%-5c", 'a');

// 	printf("\n// ===== %%p =====\n");
// 	{
// 		void *p = (void *)0x12345678;
// 		T_PTR("%p", p);
// 	}
// 	{
// 		void *p = (void *)0x1;
// 		T_PTR("%p", p);
// 	}

// 	printf("\n// ===== %%%% =====\n");
// 	printf("\t{ \"%%%%\", \"");
// 	printf("%%");
// 	printf("\" },\n");

// 	printf("\n// ===== %%f =====\n");
// 	T_FLT("%f", 0.0);
// 	T_FLT("%f", 1.5);
// 	T_FLT("%f", -1.5);
// 	T_FLT("%f", 3.14159265358979);
// 	T_FLT("%.2f", 3.14159265358979);
// 	T_FLT("%.0f", 3.5);
// 	T_FLT("%.0f", 2.5);
// 	T_FLT("%.0f", 0.99);
// 	T_FLT("%.1f", 0.15);
// 	T_FLT("%.6f", 0.1);
// 	T_FLT("%10.2f", 3.14159);
// 	T_FLT("%-10.2f", 3.14159);
// 	T_FLT("%010.2f", 3.14159);
// 	T_FLT("%010.2f", -3.14159);
// 	T_FLT("%+.2f", 3.14159);
// 	T_FLT("% .2f", 3.14159);
// 	T_FLT("%f", 100.0);
// 	T_FLT("%f", 0.001);
// 	T_FLT("%.10f", 1.0 / 3.0);

// 	printf("\n// ===== combos (width/precision dynamiques) =====\n");
// 	printf("\t{ \"%%*d\", \"");      // width dynamique = 8, val = 42
// 	printf("%*d", 8, 42);
// 	printf("\" },\n");

// 	printf("\t{ \"%%.*d\", \"");     // precision dynamique = 5, val = 42
// 	printf("%.*d", 5, 42);
// 	printf("\" },\n");

// 	printf("\t{ \"%%.*f\", \"");
// 	printf("%.*f", 5, 18.467);
// 	printf("\" },\n");

// 	printf("\t{ \"%%.*f\", \"");
// 	printf("%.*f", 2, 18.467);
// 	printf("\" },\n");

// 	printf("\t{ \"%%-*d\", \"");     // width dynamique = 8 (gauche), val = 42
// 	printf("%-*d", 8, 42);
// 	printf("\" },\n");

// 	return 0;
// }