#include "malloc.h"
#include <stdio.h>

// Find the right zone in the allocation linked list
// to match the pointer
t_zone **find_zone_link(t_zone **head, t_zone *zone)
{
	t_zone **link = head;

	while (*link && *link != zone)
		link = &(*link)->next;
	return link;
}

// Returns true if 'right' starts exactly where 'left' ends in memory.
//
// Memory layout :
//
//  left                                        right
//   |                                            |
//   v                                            v
//  [ t_block | . . . data (left->size bytes) . . | t_block | . . . ]
//   ^          ^                                 ^
//   left      left + 1         (char *)(left + 1) + left->size
//              |
//              + 1 skips sizeof(t_block) bytes (pointer arithmetic)
//
// If (char *)(left + 1) + left->size == (char *)right -> blocks are adjacent
//
bool is_adjacent(t_block *left, t_block *right)
{
	return ((char *)(left + 1) + left->size == (char *)right);
}

// Take two block and merge it together (if they are adjacent in memory and in the linked list)
t_block *merge_with_next(t_block *block, t_zone *zone)
{
	t_block *next = block->next;

	if (!next || !next->is_free || !is_adjacent(block, next))
		return block;

	block->size += sizeof(t_block) + next->size;
	block->next = next->next;

	if (block->next)
		block->next->prev = block;

	zone->free_blocks--;
	zone->total_blocks--;
	g_alloc.counter.merge_number++;
	return block;
}

// TINY or SMALL zones: marks the block as free.
// Check whether, as a result of this block being freed, the entire zone is now free,
// if so, the entire zone is freed,
// otherwise, check whether the adjacent blocks are free in order to group the blocks together (defragment the memory)
static void free_other(t_block *block, t_zone **zone_head)
{
	t_zone *zone = block->owner;
	t_zone **zone_link = find_zone_link(zone_head, zone);

	if (!*zone_link) {
		if (g_alloc.env.MALLOC_LOG_)
			printf(BLD_RED
			       "Error: ptr not recognize, free failed\n" RESET);
		return;
	}

	if (block->is_free)
		return;
	if (g_alloc.env.MALLOC_PERTURB_ENABLE_ &&
	    g_alloc.env.MALLOC_PERTURB_VALUE_)
		perturb_fill((void *)(block + 1), block->size, true);

	block->is_free = true;
	zone->free_blocks++;
	// int kind = block->kind;

	while (block->prev && block->prev->is_free &&
	       is_adjacent(block->prev, block))
		block = merge_with_next(block->prev, zone);

	while (block->next && block->next->is_free &&
	       is_adjacent(block, block->next))
		block = merge_with_next(block, zone);

	// if (zone->total_blocks == 1) {
	// 	t_zone *next = zone->next;
	// 	if (munmap(zone, zone->zone_size) < 0) {
	// 		if (g_alloc.env.MALLOC_LOG_)
	// 			printf(BLD_RED"Error: munmap() failed while dealocate %s block\n"RESET, kind == TINY ? "TINY" : "SMALL");
	// 		return ;
	// 	}
	// 	*zone_link = next;
	// }
}

// LARGE type zone: free the entire zone
static void free_large(t_block *block)
{
	t_zone *zone = block->owner;
	t_zone **zone_link = find_zone_link(&g_alloc.large, zone);

	if (!*zone_link) {
		if (g_alloc.env.MALLOC_LOG_)
			printf(BLD_RED
			       "Error: ptr not recognize, free failed\n" RESET);
		return;
	}

	t_zone *next = zone->next;

	// if (munmap(zone, zone->zone_size) < 0) {
	// 	if (g_alloc.env.MALLOC_LOG_)
	// 		printf(BLD_RED"Error: munmap() failed while dealocate LARGE block\n"RESET);
	// 	return ;
	// }
	if (g_alloc.env.mmap_max > 0)
		g_alloc.env.mmap_max--;

	if (*zone_link)
		*zone_link = next;
}

void internal_free(void *ptr)
{
	if (!ptr)
		return;

	t_block *block = ((t_block *)ptr) - 1;

	switch (block->kind) {
	case TINY:
		free_other(block, &g_alloc.tiny);
		break;
	case SMALL:
		free_other(block, &g_alloc.small);
		break;
	case LARGE:
		free_large(block);
		break;
	default:
		break;
	}
}

void free(void *ptr)
{
	// pthread_mutex_lock(&g_alloc.mutex);
	is_env_var();
	internal_free(ptr);
	// pthread_mutex_unlock(&g_alloc.mutex);
}