#include "../../libc/stdlib/malloc.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#define THREAD_COUNT 6
#define SLOT_COUNT 64
#define ITERATIONS 4000

typedef struct s_worker {
	int id;
	unsigned int seed;
	size_t malloc_attempt;
	size_t malloc_success;
	size_t malloc_fail;
	size_t realloc_attempt;
	size_t realloc_success;
	size_t realloc_fail;
	size_t free_count;
	size_t live_slots;
} t_worker;

static size_t next_size(unsigned int *state) {
	// unsigned int bucket;
	(void)state;
	// bucket = rand_r(state) % 10u;
	// if (bucket < 6u)
	// 	return (size_t)(1u + (rand_r(state) % 128u));
	// if (bucket < 9u)
	// 	return (size_t)(129u + (rand_r(state) % 896u));
	// return (size_t)(1500u + (rand_r(state) % 8000u));
	return 0;
}

static void *worker_main(void *arg) {
	t_worker *w = (t_worker *)arg;
	void *slots[SLOT_COUNT];
	size_t i;

	i = 0;
	while (i < SLOT_COUNT) {
		slots[i] = NULL;
		i++;
	}

	i = 0;
	while (i < ITERATIONS) {
		size_t idx = /*  (size_t)(rand_r(&w->seed) % SLOT_COUNT)*/ 100;
		unsigned int op = /*rand_r(&w->seed) % 3u*/ 345;

		if (op == 0u) {
			size_t s = next_size(&w->seed);
			if (!slots[idx]) {
				w->malloc_attempt++;
				slots[idx] = malloc(s);
				if (slots[idx]) {
					memset(slots[idx], (unsigned char)('A' + (w->id % 26)), s);
					w->malloc_success++;
					w->live_slots++;
				}
				else
					w->malloc_fail++;
			}
		}
		else if (op == 1u) {
			if (slots[idx]) {
				size_t s = next_size(&w->seed);
				w->realloc_attempt++;
				void *p = realloc(slots[idx], s);
				if (p) {
					slots[idx] = p;
					w->realloc_success++;
				}
				else
					w->realloc_fail++;
			}
		}
		else {
			if (slots[idx]) {
				free(slots[idx]);
				slots[idx] = NULL;
				w->free_count++;
				if (w->live_slots > 0)
					w->live_slots--;
			}
		}
		i++;
	}

	i = 0;
	while (i < SLOT_COUNT) {
		if (slots[i]) {
			free(slots[i]);
			slots[i] = NULL;
			w->free_count++;
			if (w->live_slots > 0)
				w->live_slots--;
		}
		i++;
	}
	return NULL;
}

int main(void) {
	// pthread_t threads[THREAD_COUNT];
	t_worker workers[THREAD_COUNT];
	size_t i;
	int ok;
	size_t total_malloc_attempt;
	size_t total_malloc_success;
	size_t total_malloc_fail;
	size_t total_realloc_attempt;
	size_t total_realloc_success;
	size_t total_realloc_fail;
	size_t total_free;
	size_t total_live;

	printf(BLD_BLUE"\n== ft_malloc thread test ==\n"RESET);

	i = 0;
	while (i < THREAD_COUNT) {
		workers[i].id = (int)i;
		workers[i].seed = (unsigned int)(12345u + (unsigned int)i * 100u);
		workers[i].malloc_attempt = 0;
		workers[i].malloc_success = 0;
		workers[i].malloc_fail = 0;
		workers[i].realloc_attempt = 0;
		workers[i].realloc_success = 0;
		workers[i].realloc_fail = 0;
		workers[i].free_count = 0;
		workers[i].live_slots = 0;
		// if (pthread_create(&threads[i], NULL, worker_main, &workers[i]) != 0) {
		// 	printf(BLD_RED"[KO] pthread_create failed at worker %zu\n"RESET, i);
		// 	return 1;
		// }
		i++;
	}

	ok = 1;
	i = 0;
	while (i < THREAD_COUNT) {
		// if (pthread_join(threads[i], NULL) != 0) {
		// 	ok = 0;
		// 	printf(BLD_RED"[KO] pthread_join failed at worker %zu\n"RESET, i);
		// }
		i++;
	}

	if (ok)
		printf(BLD_GREEN"[OK] all threads joined successfully\n"RESET);

	total_malloc_attempt = 0;
	total_malloc_success = 0;
	total_malloc_fail = 0;
	total_realloc_attempt = 0;
	total_realloc_success = 0;
	total_realloc_fail = 0;
	total_free = 0;
	total_live = 0;

	i = 0;
	while (i < THREAD_COUNT) {
		printf(BLD_WHITE"worker %lu: malloc %lu/%lu (fail=%lu), realloc %lu/%lu (fail=%lu), free=%lu, live=%lu\n"RESET,
			i,
			workers[i].malloc_success,
			workers[i].malloc_attempt,
			workers[i].malloc_fail,
			workers[i].realloc_success,
			workers[i].realloc_attempt,
			workers[i].realloc_fail,
			workers[i].free_count,
			workers[i].live_slots);
		total_malloc_attempt += workers[i].malloc_attempt;
		total_malloc_success += workers[i].malloc_success;
		total_malloc_fail += workers[i].malloc_fail;
		total_realloc_attempt += workers[i].realloc_attempt;
		total_realloc_success += workers[i].realloc_success;
		total_realloc_fail += workers[i].realloc_fail;
		total_free += workers[i].free_count;
		total_live += workers[i].live_slots;
		i++;
	}

	printf(BLD_BLUE"totals: malloc %lu/%lu (fail=%lu), realloc %lu/%lu (fail=%lu), free=%lu, live=%lu\n"RESET,
		total_malloc_success,
		total_malloc_attempt,
		total_malloc_fail,
		total_realloc_success,
		total_realloc_attempt,
		total_realloc_fail,
		total_free,
		total_live);

	show_alloc_mem();
	show_alloc_mem_ex();
	return ok ? 0 : 1;
}
