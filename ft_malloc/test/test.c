#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "../../libc/stdlib/malloc.h"

static int	g_pass = 0;
static int	g_fail = 0;

static void	report(const char *name, int ok) {
	if (ok) {
		g_pass++;
		printf(BLD_GREEN"[OK] "RESET"%s\n", name);
	}
	else {
		g_fail++;
		printf(BLD_RED"[KO] "RESET"%s\n", name);
	}
}

// void print_lenght_calcul() {
	// printf("align: %d\n", ALIGN8(4050));
	// size_t zone = sizeof(t_zone);
	// size_t block = sizeof(t_block);
	// size_t align_t = ALIGN8(TINY_MAX);
	// size_t align_s = ALIGN8(SMALL_MAX);
	// size_t page_size = PAGE_SIZE;
	
	// size_t res_s = zone + (align_s + block) * 100;
	// size_t res_t = zone + (align_t + block) * 100;
	// printf("calcul small:\n\t \
	// 	  (t_zone)%ld\n\t \
	// 	+ ((align)%ld + (t_block)%ld) \n\t \
	// 	* 100\n\t \
	// 	= align in memory %ld / 0x%zX / pages = %ld\n", zone, align_s, block, ALIGN_TO_PAGE(res_s), ALIGN_TO_PAGE(res_s), ALIGN_TO_PAGE(res_s) / PAGE_SIZE);
	// printf("calcul tiny:\n\t \
	// 	  (t_zone)%ld\n\t \
	// 	+ ((align)%ld + (t_block)%ld) \n\t \
	// 	* 100\n\t \
	// 	= align in memory %ld / 0x%zX / pages = %ld\n", zone, align_t, block, ALIGN_TO_PAGE(res_t), ALIGN_TO_PAGE(res_t), ALIGN_TO_PAGE(res_t) / PAGE_SIZE);
// }

void test_threshold() {
	// setenv("MALLOC_MMAP_THRESHOLD_", "100", 0);
	// setenv("MALLOC_PERTURB_", "45", 0);
	// char	*str = malloc(700);
	
	// printf("align : %zu, page align: %zu\n", ALIGN8(700), LARGE_ZONE_SIZE(700));
	// printf("size of header: zone: %zu, block: %zu\n", sizeof(t_zone), sizeof(t_block));
	
	// str = memset(str, 'A', 700);
	// show_alloc_mem_ex();
	// free(str);
	// unsetenv("MALLOC_MMAP_THRESHOLD_");
	// unsetenv("MALLOC_PERTURB_");
}

int	main(void) {
	test_threshold();
	char	*tiny;
	char	*small;
	char	*large;
	char	*r;
	void	*arr[32];
	size_t	i;

	printf(BLD_BLUE"\n== malloc test suite ==\n"RESET);

	/* Basic allocations: tiny, small, large */
	tiny = malloc(32);
	small = malloc(700);
	large = malloc(6000);
	report("malloc tiny", tiny != NULL);
	report("malloc small", small != NULL);
	report("malloc large", large != NULL);

	if (tiny)
		memset(tiny, 'A', 32);
	// if (small)
		// strcpy(small, "small-block-data");
	if (large)
		memset(large, 'L', 6000);

	show_alloc_mem();

	/* realloc grow + shrink with data preservation */
	r = realloc(tiny, 200);
	report("realloc grow tiny->small", r != NULL);
	if (r) {
		report("preserve data after grow", r[0] == 'A' && r[31] == 'A');
		tiny = r;
	}

	r = realloc(tiny, 24);
	report("realloc shrink", r != NULL);
	if (r) {
		report("preserve data after shrink", r[0] == 'A');
		tiny = r;
	}

	/* API edge cases */
	report("malloc(0) == NULL", malloc(0) == NULL);
	r = realloc(NULL, 128);
	report("realloc(NULL, n)", r != NULL);
	if (r)
		free(r);
	r = realloc(small, 0);
	report("realloc(ptr, 0) == NULL", r == NULL);
	small = NULL;

	/* small stress pass */
	i = 0;
	while (i < 32) {
		arr[i] = malloc((i + 1) * 17);
		i++;
	}
	i = 0;
	while (i < 32) {
		if (arr[i])
			free(arr[i]);
		i++;
	}
	report("stress alloc/free x32", 1);

	printf(BLD_BLUE"\n== Detailed dump before cleanup ==\n"RESET);
	show_alloc_mem_ex();

	/* Cleanup */
	if (tiny)
		free(tiny);
	if (small)
		free(small);
	if (large)
		free(large);

	printf(BLD_BLUE"\n== Detailed dump after cleanup ==\n"RESET);
	show_alloc_mem_ex();

	printf(BLD_WHITE"\nSummary: %lu passed, %lu failed\n"RESET,
		(size_t)g_pass, (size_t)g_fail);

	return (g_fail != 0);
}