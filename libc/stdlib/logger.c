#include "malloc.h"
#include <stdio.h>

// ----------------------------------------------------------------
// 						Classic Logger
// ----------------------------------------------------------------

static void print_alloc(t_zone *zone, char *type, size_t *total_mem) {
	t_block	*block;

	block = zone ? zone->block_list : NULL;
	if(block) {
		printf(BLD_WHITE"\n%s : %p\n"RESET, type, zone);
		while (block) {
			printf(BLD_WHITE"Block : %p -> %p = %lu bytes, the block is %s\n"RESET, block, (char *)(block + 1) + block->size, block->size, block->is_free == true ? "free" : "in use");
			*total_mem += block->size;
			block = block->next;
		}
	}
	else 
		printf(BLD_WHITE"\nNo %s block\n"RESET, type);
}

void	show_alloc_mem() {
	// pthread_mutex_lock(&g_alloc.mutex);
	t_zone	*tiny = g_alloc.tiny;
	t_zone	*small = g_alloc.small;
	t_zone	*large = g_alloc.large;
	size_t	total_mem = 0;

	printf(BLD_PURPLE"\n====================================================================================\n");
	printf("     MEMORY LISTING                                                                 \n");
	printf("====================================================================================\n"RESET);
	print_alloc(tiny, "TINY", &total_mem);
	print_alloc(small, "SMALL", &total_mem);
	print_alloc(large, "LARGE", &total_mem);

	printf(BLD_WHITE"\nTOTAL : %lu\n"RESET, total_mem);
	// pthread_mutex_unlock(&g_alloc.mutex);
}

// ----------------------------------------------------------------
// 						Extend Logger
// ----------------------------------------------------------------

t_frag_stats	compute_fragmentation() {
	t_frag_stats	stats = {0};
	t_zone			*zone = NULL;
	t_block			*block = NULL;

	zone = g_alloc.tiny;
	while (zone) {
		stats.total_mmap += zone->zone_size;
		block = zone->block_list;
		while (block) {
			if (block->is_free) {
				stats.total_free += block->size;
				if (block->size > stats.largest_free)
					stats.largest_free = block->size;
			}
			else
				stats.total_allocated += block->size;
			block = block->next;
		}
		zone = zone->next;
	}

	zone = g_alloc.small;
	while (zone) {
		stats.total_mmap += zone->zone_size;
		block = zone->block_list;
		while (block) {
			if (block->is_free) {
				stats.total_free += block->size;
				if (block->size > stats.largest_free)
					stats.largest_free = block->size;
			}
			else
				stats.total_allocated += block->size;
			block = block->next;
		}
		zone = zone->next;
	}

	zone = g_alloc.large;
	while (zone) {
		stats.total_mmap += zone->zone_size;
		block = zone->block_list;
		while (block) {
			if (block->is_free) {
				stats.total_free += block->size;
				if (block->size > stats.largest_free)
					stats.largest_free = block->size;
			}
			else
				stats.total_allocated += block->size;
			block = block->next;
		}
		zone = zone->next;
	}

	if (stats.total_free > 0)
		stats.ext_frag_percent = (100 * (stats.total_free - stats.largest_free)) / stats.total_free;

	return stats;
}

static void	print_hex_byte(unsigned char byte) {
	const char	*digits;

	digits = "0123456789abcdef";
	putchar(digits[byte >> 4]);
	putchar(digits[byte & 0x0F]);
	putchar(' ');
}

static void print_hex_dump(t_zone *zone, char *type, size_t print_limit) {
	t_block	*block;

	block = zone ? zone->block_list : NULL;
	if(block) {
		printf(BLD_RED"\n%s ZONE : %p - bloc in this zone %lu\n"RESET, type, zone, zone->total_blocks);
		printf(BLD_WHITE"------------------------------------------------------------------------------------\n"RESET);
		while (block) {
			printf(BLD_BLUE"Owner zone: %s  -  bloc %s  -  size: %lu\n\n"RESET, type, block->is_free == true ? "free" : "in use", block->size);
			if (block->size < print_limit) {
				unsigned char *p = (unsigned char *)(block + 1);
				for (size_t i = 0; i < block->size; i += 16) {

					printf(BLD_WHITE"%p : "BLD_GREEN, (char *)(block + 1) + i);

					for (size_t j = 0; j < 16; j++) {
						if (i + j < block->size)
							print_hex_byte(p[i + j]);
						else
							printf("   ");
					}

					printf(BLD_WHITE" | ");
					for (size_t j = 0; j < 16 && i + j < block->size; j++) {
						unsigned char c = p[i + j];
						printf("%c", (c >= 32 && c <= 126) ? c : '.');
					}
					printf(RESET"\n");
				}
			}
			else
				printf(BLD_WHITE"The bloc is too large to be dump\n"RESET);
			printf(BLD_WHITE"\n------------------------------------------------------------------------------------\n"RESET);
			block = block->next;
		}
	}
	else 
		printf(BLD_WHITE"\nNo %s block to dump\n"RESET, type);
}

void	show_alloc_mem_ex() {
	// pthread_mutex_lock(&g_alloc.mutex);
	t_zone	*tiny = g_alloc.tiny;
	t_zone	*small = g_alloc.small;
	t_zone	*large = g_alloc.large;
	size_t	print_limit = 5000;
	t_frag_stats stats;
	
	printf(BLD_PURPLE"\n====================================================================================\n");
	printf("     MEMORY HEX DUMP  -  Total mmap call %lu  -  dump limit %lu bytes               \n", g_alloc.counter.mmap_total, print_limit);
	printf("====================================================================================\n"RESET);
	print_hex_dump(tiny, "TINY", print_limit);
	print_hex_dump(small, "SMALL", print_limit);
	print_hex_dump(large, "LARGE", print_limit);

	stats = compute_fragmentation();
	printf(BLD_PURPLE"\n====================================================================================\n");
	printf("     FRAGMENTATION & STATS                                                          \n");
	printf("====================================================================================\n"RESET);
	printf(BLD_WHITE"Total mmap'd:       %lu bytes\n"RESET, stats.total_mmap);
	printf(BLD_GREEN"Total allocated:    %lu bytes\n"RESET, stats.total_allocated);
	printf(BLD_BLUE"Total free:         %lu bytes\n"RESET, stats.total_free);
	printf(BLD_WHITE"Largest free block: %lu bytes\n"RESET, stats.largest_free);
	printf(BLD_RED"Fragmentation:      %lu%%\n"RESET, stats.ext_frag_percent);
	printf(BLD_WHITE"Split operations:   %lu\n"RESET, g_alloc.counter.split_number);
	printf(BLD_WHITE"Merge operations:   %lu\n"RESET, g_alloc.counter.merge_number);
	printf(BLD_PURPLE"====================================================================================\n"RESET);

	// pthread_mutex_unlock(&g_alloc.mutex);
}
