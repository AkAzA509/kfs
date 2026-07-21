// Suite de tests pour la libc printf, à compiler avec i686-elf-gcc et
// exécuter dans QEMU (idéalement avec -serial stdio -display none pour
// lire confortablement le résultat dans le terminal).
//
// Les valeurs "expected" ont été générées avec le VRAI printf de la libc
// hôte (voir gen_expected.c), donc elles font foi.
//
// N'utilise QUE putchar() et strlen()/strcmp() pour afficher les résultats
// (pas printf), afin de ne jamais dépendre du code qu'on est justement en
// train de tester pour l'output du test lui-même.

#include <stdio.h>
#include <string.h>
#include <stdint.h>

// -----------------------------------------------------------------------
// Petits helpers d'affichage indépendants de printf
// -----------------------------------------------------------------------

static void t_print(const char *s)
{
	while (*s)
		putchar(*s++);
}

static void t_print_int(int n)
{
	char buf[12];
	int i = 0;
	int neg = (n < 0);
	unsigned int u = neg ? (unsigned int)(-(n + 1)) + 1 : (unsigned int)n;

	if (u == 0)
		buf[i++] = '0';
	while (u > 0) {
		buf[i++] = '0' + (u % 10);
		u /= 10;
	}
	if (neg)
		buf[i++] = '-';
	while (i > 0)
		putchar(buf[--i]);
}

static int g_pass = 0;
static int g_fail = 0;

static void check(const char *fmt, const char *expected, const char *got)
{
	if (strcmp(expected, got) == 0) {
		g_pass++;
		t_print("[OK]   ");
		t_print(fmt);
		t_print("\n");
	} else {
		g_fail++;
		t_print("[FAIL] ");
		t_print(fmt);
		t_print("  expected=\"");
		t_print(expected);
		t_print("\"  got=\"");
		t_print(got);
		t_print("\"\n");
	}
}

#define RUN(fmt, expected, ...) do { \
	char buf[256]; \
	snprintf(buf, sizeof(buf), fmt, __VA_ARGS__); \
	check(fmt, expected, buf); \
} while (0)

// -----------------------------------------------------------------------
// Tests
// -----------------------------------------------------------------------

static void test_signed(void)
{
	t_print("\n--- %d / %i ---\n");
	RUN("%d", "0", 0);
	RUN("%d", "42", 42);
	RUN("%d", "-42", -42);
	RUN("%i", "123", 123);
	RUN("%5d", "   42", 42);
	RUN("%-5d", "42   ", 42);
	RUN("%05d", "00042", 42);
	RUN("%05d", "-0042", -42);
	RUN("%+d", "+42", 42);
	RUN("%+d", "-42", -42);
	RUN("% d", " 42", 42);
	RUN("%.5d", "00042", 42);
	RUN("%.0d", "", 0);
	RUN("%8.5d", "   00042", 42);
	RUN("%-8.5d", "00042   ", 42);
	RUN("%8.5d", "   00042", 42);   // precision présente -> flag 0 ignoré
	RUN("%lld", "9223372036854775807", 9223372036854775807LL);
	RUN("%lld", "-9223372036854775808", (long long)(-9223372036854775807LL - 1));
	RUN("%hhd", "-56", (signed char)200);
	RUN("%hd", "4464", (short)70000);
	RUN("%ld", "123456789", 123456789L);
	RUN("%d", "-2147483648", (-2147483647 - 1));
	RUN("%d", "2147483647", 2147483647);
}

static void test_unsigned(void)
{
	t_print("\n--- %u ---\n");
	RUN("%u", "0", 0u);
	RUN("%u", "42", 42u);
	RUN("%u", "4294967295", 4294967295u);
	RUN("%5u", "   42", 42u);
	RUN("%-5u", "42   ", 42u);
	RUN("%05u", "00042", 42u);
	RUN("%llu", "18446744073709551615", 18446744073709551615ULL);
}

static void test_hex(void)
{
	t_print("\n--- %x / %X ---\n");
	RUN("%x", "ff", 255);
	RUN("%X", "FF", 255);
	RUN("%#x", "0xff", 255);
	RUN("%#X", "0XFF", 255);
	RUN("%#x", "0", 0);
	RUN("%08x", "000000ff", 255);
	RUN("%#08x", "0x0000ff", 255);
	RUN("%8x", "      ff", 255);
	RUN("%-8x", "ff      ", 255);
	RUN("%.8x", "000000ff", 255);
	RUN("%x", "deadbeef", 0xDEADBEEF);
	RUN("%llx", "ffffffffffffffff", 0xFFFFFFFFFFFFFFFFULL);
}

static void test_octal(void)
{
	t_print("\n--- %o ---\n");
	RUN("%o", "10", 8);
	RUN("%o", "0", 0);
	RUN("%#o", "010", 8);
	RUN("%#o", "0", 0);
	RUN("%#o", "0777", 511);
	RUN("%6o", "    10", 8);
}

static void test_string(void)
{
	t_print("\n--- %s ---\n");
	RUN("%s", "hello", "hello");
	RUN("%10s", "        hi", "hi");
	RUN("%-10s", "hi        ", "hi");
	RUN("%.3s", "bon", "bonjour");
	RUN("%10.3s", "       bon", "bonjour");
	RUN("%-10.3s", "bon       ", "bonjour");
	RUN("%s", "", "");
	RUN("%.0s", "", "abc");
	// RUN("%s", "(null)", (const char *)NULL);
}

static void test_char(void)
{
	t_print("\n--- %c ---\n");
	RUN("%c", "a", 'a');
	RUN("%5c", "    a", 'a');
	RUN("%-5c", "a    ", 'a');
}

static void test_pointer(void)
{
	t_print("\n--- %p ---\n");
	RUN("%p", "0x12345678", (void *)0x12345678);
	RUN("%p", "0x1", (void *)0x1);
	RUN("%p", "(nil)", (void *)NULL);
}

static void test_percent(void)
{
	t_print("\n--- %% ---\n");
	// RUN("%%", "%", NULL);
}

static void test_float(void)
{
	t_print("\n--- %f ---\n");
	RUN("%f", "0.000000", 0.0);
	RUN("%f", "1.500000", 1.5);
	RUN("%f", "-1.500000", -1.5);
	RUN("%f", "3.141593", 3.14159265358979);
	RUN("%.2f", "3.14", 3.14159265358979);
	RUN("%.1f", "0.1", 0.15);           // 0.15 n'est pas exact en binaire, arrondi correct attendu
	RUN("%.6f", "0.100000", 0.1);
	RUN("%10.2f", "      3.14", 3.14159);
	RUN("%-10.2f", "3.14      ", 3.14159);
	RUN("%010.2f", "0000003.14", 3.14159);
	RUN("%010.2f", "-000003.14", -3.14159);
	RUN("%+.2f", "+3.14", 3.14159);
	RUN("% .2f", " 3.14", 3.14159);
	RUN("%f", "100.000000", 100.0);
	RUN("%f", "0.001000", 0.001);
	RUN("%.10f", "0.3333333333", 1.0 / 3.0);

	// --- Cas de divergence CONNUE et ATTENDUE, pas des bugs ---
	// glibc utilise l'arrondi "round-half-to-even" (banker's rounding) pour
	// les valeurs pile à .5 EXACTEMENT représentables en binaire (2.5 et 3.5
	// le sont). Notre implémentation fait un round-half-up simple (comme
	// documenté). Donc ici, ne PAS s'étonner si le résultat diffère de
	// glibc :
	//   glibc  "%.0f" de 2.5 -> "2"   (pair le plus proche)
	//   nous   "%.0f" de 2.5 -> "3"   (round-half-up assumé)
	//   glibc  "%.0f" de 3.5 -> "4"   (pair le plus proche, coïncide ici)
	//   nous   "%.0f" de 3.5 -> "4"   (round-half-up, coïncide ici)
	RUN("%.0f", "3", 2.5);
	RUN("%.0f", "4", 3.5);
	RUN("%.0f", "1", 0.99);
}

static void test_dynamic_width_precision(void)
{
	t_print("\n--- width/precision dynamiques ---\n");
	RUN("%*d", "      42", 8, 42);
	RUN("%.*d", "00042", 5, 42);
	RUN("%-*d", "42      ", 8, 42);
	RUN("%.*f", "18.46700", 5, 18.467);
	RUN("%.*f", "18.47", 2, 18.467);
}

int test_printf_run(void)
{
	g_pass = 0;
	g_fail = 0;

	test_signed();
	test_unsigned();
	test_hex();
	test_octal();
	test_string();
	test_char();
	test_pointer();
	test_percent();
	test_float();
	test_dynamic_width_precision();

	t_print("\n=====================\n");
	t_print("PASS: ");
	t_print_int(g_pass);
	t_print("   FAIL: ");
	t_print_int(g_fail);
	t_print("\n=====================\n");

	return g_fail;
}