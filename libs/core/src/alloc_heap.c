/**
 * @file alloc_heap.c
 * @brief Heap allocator implementation using system malloc/free.
 *
 * This file implements a heap allocator that wraps the system's malloc/free
 * functions, providing aligned memory allocation capabilities. It uses platform-specific
 * aligned allocation functions (aligned_alloc on Linux, _aligned_malloc on Windows)
 * and includes debugging features like memory tagging for use-after-free detection.
 */

#include "alloc_internal.h"
#include "core_alloc.h"
#include "core_diag.h"
#include "core_types.h"

#include <stdlib.h>

/** @brief Memory tag value used to mark freed memory for debugging. */
#define freed_mem_tag 0xFF

/**
 * @brief Heap allocator structure.
 *
 * This structure contains only the base allocator interface since the heap
 * allocator doesn't need to maintain any internal state beyond the system
 * malloc/free functionality.
 */
struct AllocatorHeap {
    Allocator api;   /**< Base allocator interface */
};

/**
 * @brief Allocate aligned memory from the system heap.
 *
 * This function allocates memory with the specified size and alignment using
 * platform-specific aligned allocation functions. On Linux, it uses aligned_alloc,
 * and on Windows, it uses _aligned_malloc.
 *
 * @param allocator The heap allocator instance (unused)
 * @param size Size of memory to allocate in bytes
 * @param align Alignment requirement for the allocation
 * @return Mem structure containing the allocated memory, or null memory if allocation failed
 */
static Mem alloc_heap_alloc(Allocator* allocator, const usize size, const usize align) {
    (void)allocator;


#if defined(ATLAS_LINUX)
	return mem_create(aligned_alloc(align, size), size);
#elif defined(ATLAS_WIN32)
	return mem_create(_aligned_malloc(size, align), size);
#else
	_Static_assert(false, "Unsupported platform")
#endif
}

/**
 * @brief Free memory back to the system heap.
 *
 * This function frees memory that was previously allocated by the heap allocator.
 * For debugging purposes, it first tags the memory with a special pattern to help
 * detect use-after-free bugs, then calls the appropriate platform-specific free function.
 *
 * @param allocator The heap allocator instance (unused)
 * @param mem The memory block to free
 */
static void alloc_heap_free(Allocator* allocator, Mem mem) {
    (void)allocator;

    diag_assert(mem_valid(mem));
    mem_set(mem, freed_mem_tag); // Basic tag to detect use-after-free

#if defined(ATLAS_LINUX)
	free(mem.ptr);
#elif defined(ATLAS_WIN32)
	_aligned_free(mem.ptr);
#else
	_Static_assert(false, "Unsupported platform");
#endif
}

/**
 * @brief Get the minimum allocation size for the heap allocator.
 *
 * The heap allocator can allocate any size starting from 1 byte.
 *
 * @param allocator The heap allocator instance (unused)
 * @return The minimum allocation size (1 byte)
 */
static usize alloc_heap_min_size(Allocator* allocator) {
    (void)allocator;

    return 1;
}

/**
 * @brief Get the maximum allocation size for the heap allocator.
 *
 * The heap allocator can theoretically allocate up to the maximum size_t value,
 * limited only by available system memory.
 *
 * @param allocator The heap allocator instance (unused)
 * @return The maximum allocation size (usize_max)
 */
static usize alloc_heap_max_size(Allocator* allocator) {
    (void)allocator;

    return usize_max;
}

/** @brief Global instance of the heap allocator. */
static struct AllocatorHeap g_allocatorIntern;

/**
 * @brief Initialize the global heap allocator.
 *
 * This function initializes the global heap allocator instance with the appropriate
 * function pointers for heap allocation operations. The allocator wraps the system's
 * malloc/free functionality with proper alignment support.
 *
 * @return Pointer to the initialized heap allocator
 */
Allocator* alloc_heap_init() {
    g_allocatorIntern = (struct AllocatorHeap) {
        (Allocator) {
            .alloc = alloc_heap_alloc,
            .free = alloc_heap_free,
            .minSize = alloc_heap_min_size,
            .maxSize = alloc_heap_max_size,
        },
    };

    return (Allocator*)&g_allocatorIntern;
}