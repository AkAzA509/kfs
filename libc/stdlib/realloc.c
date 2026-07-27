#include "malloc.h"
#include <stdio.h>
#include <string.h>

// Try to merge adjacent bloc to his right to grab free memory,
// if it work, try to split the block
static bool	merge_block(t_block *block, size_t size, t_zone *zone)
{
	while (block && block->next && block->next->is_free && is_adjacent(block, block->next))
		block = merge_with_next(block, zone);

	if (block->size >= size) {
		if (split_block(block, size)) {
			zone->total_blocks++;
			zone->free_blocks++;
		}
		return true;
	}
	return false;
}


// TINY or SMALL zones:
// if the ptr need to be shrink, ft_realloc() try to split the bloc if it failed free and recreate the ptr with size.
// if the ptr need to be growth, ft_realloc() try to merge adjacent block to grap memory (if it work try to split if the blog is really to large),
// else free and recreate the ptr too.
void	*realloc_other(t_block *block, t_zone **zone_head, void *ptr, size_t size)
{
	t_zone	*zone = block->owner;
	t_zone	**zone_match = find_zone_link(zone_head, zone);
	
	if (!*zone_match)
		return NULL;
	
	if (block->size > size) {
		if (!split_block(block, size)) {
			void *new_ptr = internal_malloc(size);
			if (!new_ptr)
				return NULL;
			
			size_t copy_size = block->size > size ? size : block->size;
			new_ptr = memcpy(new_ptr, ptr, copy_size);
			internal_free(ptr);
			return new_ptr;
		}
		zone->total_blocks++;
		zone->free_blocks++;
	}
	else {
		if (!merge_block(block, size, zone)) {
			void *new_ptr = internal_malloc(size);
			if (!new_ptr)
				return NULL;
			
			size_t copy_size = block->size > size ? size : block->size;
			new_ptr = memcpy(new_ptr, ptr, copy_size);
			internal_free(ptr);
			return new_ptr;
		}
	}
	return ptr;
}


// LARGE type zone: free the zone and remalloc a new ptr.
void	*realloc_large(t_block *block, void *ptr, size_t size)
{
	void *new_ptr = internal_malloc(size);
	if (!new_ptr)
		return NULL;
	
	size_t copy_size = block->size > size ? size : block->size;
	new_ptr = memcpy(new_ptr, ptr, copy_size);
	internal_free(ptr);
	return new_ptr;
}


// If a reallocation became too large or too small within is origin type zone
// ft_realloc() free the ptr en recreate it with ft_malloc() to have the right type zone
static void	*change_class(void *ptr, size_t new_size, size_t old_size)
{
	void *new_ptr = internal_malloc(new_size);
	if (!new_ptr)
		return NULL;
	
	size_t copy_size = old_size > new_size ? new_size : old_size;
	new_ptr = memcpy(new_ptr, ptr, copy_size);
	internal_free(ptr);
	return new_ptr;
}

static void	*internal_realloc(void *ptr, size_t new_size)
{
	if (new_size <= (size_t)0 && ptr)
		return internal_free(ptr), NULL;
	else if (!ptr && new_size > (size_t)0)
		return ptr = internal_malloc(new_size);
	else if (!ptr && new_size == (size_t)0)
		return NULL;

	size_t size = ALIGN8(new_size);

	t_block	*block = ((t_block *)ptr) - 1;

	if (block->size == size)
		return ptr;

	if((block->kind == TINY && size > TINY_MAX) || (block->kind == SMALL && (size > SMALL_MAX || size <= TINY_MAX)) || (block->kind == LARGE && size <= SMALL_MAX))
		return ptr = change_class(ptr, size, block->size);

	switch (block->kind) {
		case TINY: ptr = realloc_other(block, &g_alloc.tiny, ptr, size); break;
		case SMALL: ptr = realloc_other(block, &g_alloc.small, ptr, size); break;
		case LARGE: ptr = realloc_large(block, ptr, size); break;
		default: break;
	}

	return ptr;
}

void	*realloc(void *ptr, size_t size)
{
	void	*r_ptr = NULL;

	// pthread_mutex_lock(&g_alloc.mutex);
	is_env_var();
	r_ptr = internal_realloc(ptr, size);
	// pthread_mutex_unlock(&g_alloc.mutex);

	return r_ptr;
}