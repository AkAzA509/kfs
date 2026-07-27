#ifndef STDLIB_H
#define STDLIB_H

#include <stddef.h>

// The labs() function compute the absolute value of
// the argument x of the appropriate long type for the function.
long	labs(long x);

// The malloc() function allocates size bytes and returns a pointer to
// the allocated memory. The memory is not initialized. If size is 0,
// then malloc() returns NULL.
void	*malloc(size_t size)
	__attribute__((malloc, warn_unused_result));

// The free() function frees the memory space pointed to by ptr, which
// must have been returned by a previous call to malloc() or related
// functions. Otherwise, or if ptr has already been freed, undefined be‐
// havior occurs. If ptr is NULL, no operation is performed.
void	free(void *ptr);

// The calloc() function allocates memory for an array of nmemb elements
// of size bytes each and returns a pointer to the allocated memory. The
// memory is set to zero. If nmemb or size is 0, then calloc() returns a
// unique pointer value that can later be successfully passed to free().
// If the multiplication of nmemb and size would result in integer over‐
// flow, then calloc() returns an error. By contrast, an integer over‐
// flow would not be detected in the following call to malloc(), with the
// result that an incorrectly sized block of memory would be allocated:
// malloc(nmemb * size);
void	*calloc(size_t nmeb, size_t size);
	
// The realloc() function changes the size of the memory block pointed to
// by ptr to size bytes. The contents of the memory will be unchanged in
// the range from the start of the region up to the minimum of the old
// and new sizes. If the new size is larger than the old size, the added
// memory will not be initialized.
// If ptr is NULL, then the call is equivalent to malloc(size), for all
// values of size.
// If size is equal to zero, and ptr is not NULL, then the call is equiv‐
// alent to free(ptr).
void	*realloc(void *ptr, size_t size)
	__attribute__(());

#endif // STDLIB_H