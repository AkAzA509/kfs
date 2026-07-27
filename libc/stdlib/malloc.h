#ifndef MALLOC_H
#define MALLOC_H

// #include <stdlib.h>
// #include <sys/mman.h>
#include <unistd.h>
#include <stdbool.h>
#include <stddef.h>
// #include <pthread.h>

#define GREEN "\033[92m"
#define WHITE "\033[97m"
#define BLUE "\033[94m"
#define RED "\033[91m"
#define PURPLE "\033[38;2;255;105;255m"
#define BOLD "\033[1m"
#define RESET "\033[0m"

#define BLD_WHITE BOLD WHITE
#define BLD_RED BOLD RED
#define BLD_BLUE BOLD BLUE
#define BLD_GREEN BOLD GREEN
#define BLD_PURPLE BOLD PURPLE


// Lenght max
// TINY  : 1 to TINY_MAX octets
// SMALL : TINY_MAX+1 to SMALL_MAX
// LARGE : SMALL_MAX+1 to space limits
#define TINY_MAX	128
#define SMALL_MAX	1024


// Get the page size on the current os
// _SC_PAGESIZE is not portable (this alocator is linux only implementation)
#define PAGE_SIZE /*(size_t)sysconf(_SC_PAGESIZE)*/ 4096

// Rounds x up to the nearest multiple of 8.
// Ensures every address returned to the user is 8-byte aligned.
//
//   + 7        ensures we round up and not down
//   & ~7       clears the 3 lower bits (the actual rounding)
//
// Example with x = 13 :
//   13 + 7 = 20
//   20 & ~7 = 16
#define ALIGN8(x) (((x) + 7) & ~7)

// Rounds x up to the nearest multiple of PAGE_SIZE.
//
// The trick works because PAGE_SIZE is a power of 2 :
//   + (PAGE_SIZE - 1)  ensures we round up and not down
//   & ~(PAGE_SIZE - 1) clears the lower bits (the actual rounding)
//
// Example with PAGE_SIZE = 4096 and x = 5000 :
//   5000 + 4095 = 9095
//   9095 & ~4095 = 8192  (2 pages)
#define ALIGN_TO_PAGE(x) (((x) + PAGE_SIZE - 1) & ~(PAGE_SIZE - 1))


// Compute the zone size with the mmory alignement in one pass
// Only the SMALL and TINY are compute because the LARGE doesn't have a explicit limit size
#define TINY_ZONE_SIZE ALIGN_TO_PAGE(sizeof(t_zone) + (ALIGN8(TINY_MAX) + sizeof(t_block)) * 100)
#define SMALL_ZONE_SIZE ALIGN_TO_PAGE(sizeof(t_zone) + (ALIGN8(SMALL_MAX) + sizeof(t_block)) * 100)
#define LARGE_ZONE_SIZE(x) ALIGN_TO_PAGE(sizeof(t_zone) + (ALIGN8(x) + sizeof(t_block)))


typedef struct s_env_value {
	bool			enabled;
	size_t			value;
} 					t_env_value;

typedef enum e_env_var {
	MALLOC_PERTURB_,
	MALLOC_MMAP_MAX_,
	MALLOC_MMAP_THRESHOLD_,
	MALLOC_LOG_
}	e_env_var;

typedef enum e_block_kind {
	TINY,
	SMALL,
	LARGE
}	e_block_kind;

typedef struct s_block {
	struct s_block	*next;
	struct s_block	*prev;
	struct s_zone	*owner;
	e_block_kind	kind;
	size_t			size;
	bool			is_free;
}					t_block;

typedef struct s_zone {
	struct s_zone	*next;
	struct s_block	*block_list;
	size_t			zone_size;
	size_t			free_blocks;
	size_t			total_blocks;
}					t_zone;

typedef struct s_env {
	size_t			MALLOC_MMAP_MAX_VALUE_;			// Cap the number of large allocation
	size_t			MALLOC_MMAP_THRESHOLD_VALUE_;	// Define the min size where the alloc become a large
	size_t			MALLOC_PERTURB_VALUE_;			// Fill the allocate block with random data, at free too
	size_t			mmap_max;
	bool			MALLOC_MMAP_THRESHOLD_ENABLE_;
	bool			MALLOC_MMAP_MAX_ENABLE_;
	bool			MALLOC_PERTURB_ENABLE_;
	bool			MALLOC_LOG_;					// Enable the logging of the error
}					t_env;

typedef struct s_counter {
	size_t			mmap_total;
	size_t			split_number;
	size_t			merge_number;
}					t_counter;

typedef struct s_frag_stats {
	size_t			total_mmap;
	size_t			total_allocated;
	size_t			total_free;
	size_t			largest_free;
	size_t			ext_frag_percent;
}					t_frag_stats;


// Global struct
// Store the 3 allocation linked list
typedef struct s_allocator {
	t_zone			*tiny;
	t_zone			*small;
	t_zone			*large;
	t_env			env;
	t_counter		counter;
	// pthread_mutex_t mutex;
	
}					t_allocator;

extern t_allocator g_alloc;

// utils
t_zone			**find_zone_link(t_zone **head, t_zone *zone);
bool			split_block(t_block *block, size_t align_mem);
t_block			*merge_with_next(t_block *block, t_zone *zone);
bool			is_adjacent(t_block *left, t_block *right);
void			perturb_fill(void *ptr, size_t len, bool inverted);
void			is_env_var();
t_frag_stats	compute_fragmentation();

// internal implementation
void			internal_free(void *ptr);
void			*internal_malloc(size_t size);

// mandatory

void			*ft_malloc(size_t size);
void			ft_free(void *ptr);
void			*ft_realloc(void *ptr, size_t size);
void			show_alloc_mem();

// bonus
void			show_alloc_mem_ex();

#endif // MALLOC_H