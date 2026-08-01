#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stddef.h>
#include <stdarg.h>
#include <unistd.h>
#include <fcntl.h>

extern int kfs_vprintf(const char *restrict fmt, va_list ap)
	__attribute__((format(printf, 1, 0)));

extern int kfs_vfprintf(FILE *restrict stream, const char *restrict fmt, va_list ap)
	__attribute__((nonnull(1), format(printf, 2, 0)));

extern int kfs_vdprintf(int fd, const char *restrict fmt, va_list ap)
	__attribute__((format(printf, 2, 0)));

extern int kfs_vsprintf(char *restrict str, const char *restrict fmt, va_list ap)
	__attribute__((nonnull(1), format(printf, 2, 0)));

extern int kfs_vsnprintf(char *restrict str, size_t size, const char *restrict fmt, va_list ap)
	__attribute__((format(printf, 3, 0)));


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

static void check_results(const char *fmt, int ret_libc, int ret_kfs,
                          const char *buf_libc, const char *buf_kfs)
{
	if (ret_libc == ret_kfs && strcmp(buf_libc, buf_kfs) == 0) {
		g_pass++;
		t_print("[OK]   ");
		t_print(fmt);
		t_print("\n");
	} else {
		g_fail++;
		t_print("[FAIL] ");
		t_print(fmt);
		t_print("  libc_ret=");
		t_print_int(ret_libc);
		t_print(" kfs_ret=");
		t_print_int(ret_kfs);
		t_print("  libc_str=\"");
		t_print(buf_libc);
		t_print("\"  kfs_str=\"");
		t_print(buf_kfs);
		t_print("\"\n");
	}
}

typedef void (*test_runner_t)(const char *fmt, ...);

// --- Runner ---

static void run_sprintf(const char *fmt, ...)
{
	char b_libc[512] = {0}, b_kfs[512] = {0};
	va_list a1, a2;
	va_start(a1, fmt); va_copy(a2, a1);
	int r_libc = vsprintf(b_libc, fmt, a1);
	int r_kfs = kfs_vsprintf(b_kfs, fmt, a2);
	va_end(a1); va_end(a2);
	check_results(fmt, r_libc, r_kfs, b_libc, b_kfs);
}

static void run_snprintf(const char *fmt, ...)
{
	char b_libc[512] = {0}, b_kfs[512] = {0};
	va_list a1, a2;
	va_start(a1, fmt); va_copy(a2, a1);
	int r_libc = vsnprintf(b_libc, sizeof(b_libc), fmt, a1);
	int r_kfs = kfs_vsnprintf(b_kfs, sizeof(b_kfs), fmt, a2);
	va_end(a1); va_end(a2);
	check_results(fmt, r_libc, r_kfs, b_libc, b_kfs);
}

static void run_dprintf(const char *fmt, ...)
{
	char b_libc[512] = {0}, b_kfs[512] = {0};
	FILE *f1 = tmpfile(), *f2 = tmpfile();
	int fd1 = fileno(f1), fd2 = fileno(f2);
	va_list a1, a2;

	va_start(a1, fmt); va_copy(a2, a1);
	int r_libc = vdprintf(fd1, fmt, a1);
	int r_kfs = kfs_vdprintf(fd2, fmt, a2);
	va_end(a1); va_end(a2);

	rewind(f1); rewind(f2);
	size_t dummy1 = fread(b_libc, 1, sizeof(b_libc) - 1, f1); (void)dummy1;
	size_t dummy2 = fread(b_kfs, 1, sizeof(b_kfs) - 1, f2); (void)dummy2;
	fclose(f1); fclose(f2);

	check_results(fmt, r_libc, r_kfs, b_libc, b_kfs);
}

static void run_fprintf(const char *fmt, ...)
{
	char b_libc[512] = {0};
	char b_kfs[512] = {0};
	
	// 1. On crée un pipe UNIX pour intercepter le write() de ton Kernel
	int pipefd[2];
	if (pipe(pipefd) < 0) return;

	// 2. Pour la libc hôte : on utilise tmpfile() normalement
	FILE *f1 = tmpfile();

	va_list a1, a2;
	va_start(a1, fmt);
	va_copy(a2, a1);

	// --- Exécution LIBC ---
	int r_libc = vfprintf(f1, fmt, a1);
	fflush(f1);
	rewind(f1);
	fread(b_libc, 1, sizeof(b_libc) - 1, f1);
	fclose(f1);

	// --- Exécution KFS ---
	// Ton code fait *stream pour récupérer le FD, donc on lui passe &pipefd[1] !
	int fake_stream = pipefd[1]; 
	int r_kfs = kfs_vfprintf((FILE *)&fake_stream, fmt, a2);

	va_end(a1);
	va_end(a2);

	// On ferme le côté écriture du pipe pour débloquer la lecture
	close(pipefd[1]);

	// On lit ce que ton _vfprint_e() a écrit via write()
	ssize_t bytes_read = read(pipefd[0], b_kfs, sizeof(b_kfs) - 1);
	if (bytes_read > 0)
		b_kfs[bytes_read] = '\0';
	close(pipefd[0]);

	// --- Comparaison ---
	check_results(fmt, r_libc, r_kfs, b_libc, b_kfs);
}

static void run_printf(const char *fmt, ...)
{
	char b_libc[512] = {0}, b_kfs[512] = {0};
	FILE *f1 = tmpfile(), *f2 = tmpfile();
	int stdout_backup = dup(STDOUT_FILENO);
	va_list a1, a2;

	va_start(a1, fmt); va_copy(a2, a1);

	fflush(stdout);
	dup2(fileno(f1), STDOUT_FILENO);
	int r_libc = vprintf(fmt, a1);
	fflush(stdout);

	dup2(fileno(f2), STDOUT_FILENO);
	int r_kfs = kfs_vprintf(fmt, a2);
	fflush(stdout);

	dup2(stdout_backup, STDOUT_FILENO);
	close(stdout_backup);

	va_end(a1); va_end(a2);

	rewind(f1); rewind(f2);
	size_t dummy1 = fread(b_libc, 1, sizeof(b_libc) - 1, f1); (void)dummy1;
	size_t dummy2 = fread(b_kfs, 1, sizeof(b_kfs) - 1, f2); (void)dummy2;
	fclose(f1); fclose(f2);

	check_results(fmt, r_libc, r_kfs, b_libc, b_kfs);
}

// --- Testing blocs ---

static void test_signed(test_runner_t run)
{
	t_print("\n--- %d / %i ---\n");
	run("%d", 0);
	run("%d", 42);
	run("%d", -42);
	run("%i", 123);
	run("%5d", 42);
	run("%-5d", 42);
	run("%05d", 42);
	run("%05d", -42);
	run("%+d", 42);
	run("%+d", -42);
	run("% d", 42);
	run("%.5d", 42);
	run("%.0d", 0);
	run("%8.5d", 42);
	run("%-8.5d", 42);
	run("%lld", 9223372036854775807LL);
	run("%lld", (long long)(-9223372036854775807LL - 1));
	run("%hhd", (signed char)200);
	run("%hd", (short)70000);
	run("%ld", 123456789L);
	run("%d", (-2147483647 - 1));
	run("%d", 2147483647);
}

static void test_unsigned_and_hex(test_runner_t run)
{
	t_print("\n--- %u / %x / %X / %o ---\n");
	run("%u", 0u);
	run("%u", 4294967295u);
	run("%x", 255);
	run("%X", 255);
	run("%#x", 255);
	run("%#X", 255);
	run("%08x", 255);
	run("%o", 511);
	run("%#o", 511);
}

static void test_string_and_char(test_runner_t run)
{
	t_print("\n--- %s / %c / %p ---\n");
	run("%s", "hello KFS");
	run("%10s", "hi");
	run("%-10s", "hi");
	run("%.3s", "bonjour");
	run("%c", 'A');
	run("%5c", 'Z');
	run("%p", (void *)0x12345678);
	run("%p", (void *)NULL);
}

static void test_float(test_runner_t run)
{
	t_print("\n--- %f ---\n");
	run("%f", 0.0);
	run("%f", 1.5);
	run("%f", -1.5);
	run("%f", 3.14159265358979);
	run("%.2f", 3.14159265358979);
	run("%.1f", 0.15);
	run("%.6f", 0.1);
	run("%10.2f", 3.14159);
	run("%-10.2f", 3.14159);
	run("%010.2f", 3.14159);
	run("%010.2f", -3.14159);
	run("%+.2f", 3.14159);
	run("% .2f", 3.14159);
	run("%f", 100.0);
	run("%f", 0.001);
	run("%.10f", 1.0 / 3.0);
	run("%.0f", 3.5);
	run("%.0f", 0.99);
}

static void test_dynamic(test_runner_t run)
{
	t_print("\n--- Dynamic Width & Precision (*)---\n");
	run("%*d", 8, 42);
	run("%.*d", 5, 42);
	run("%-*d", 8, 42);
}

static void run_suite(test_runner_t runner, const char *suite_name)
{
	t_print("\n========================================\n");
	t_print(" RUNNING SUITE: ");
	t_print(suite_name);
	t_print("\n========================================\n");

	test_signed(runner);
	test_unsigned_and_hex(runner);
	test_string_and_char(runner);
	test_float(runner);
	test_dynamic(runner);
}

#define GREEN "\033[92m"
#define BLUE "\033[94m"
#define RED "\033[91m"
#define RESET "\033[0m"

void test_printf()
{
	t_print(BLUE"========================================\n");
	t_print(" PRINTF TEST SUITE\n");
	t_print("========================================\n"RESET);
	// t_print(RESET);
	run_suite(run_printf, "printf");
	run_suite(run_fprintf, "fprintf");
	run_suite(run_dprintf, "dprintf");
	run_suite(run_sprintf, "sprintf");
	run_suite(run_snprintf, "snprintf");

	t_print(g_fail > 0 ? RED : GREEN);
	t_print("\n========================================\n");
	t_print("TOTAL PASS: ");
	t_print_int(g_pass);
	t_print("   TOTAL FAIL: ");
	t_print_int(g_fail);
	t_print("\n========================================\n"RESET);
	// t_print(RESET);
}

int main(void)
{
	g_pass = 0;
	g_fail = 0;

	test_printf();

	return g_fail != 0;
}