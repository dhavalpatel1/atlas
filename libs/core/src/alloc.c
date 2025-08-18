/**
 * @file alloc.c
 * @brief Core memory allocator interface and global allocator management.
 *
 * This file provides the main interface for memory allocation in the Atlas core library.
 * It manages global allocators for heap, page, and thread-local scratch memory, and
 * provides type-safe wrappers around the allocator function pointers with proper
 * validation and debugging support.
 */

#include "alloc_internal.h"
#include "core_alloc.h"
#include "core_annotation.h"
#include "core_bits.h"
#include "core_diag.h"
#include "core_string.h"
#include "init_internal.h"

/** @brief Maximum allowed allocation size (128 MiB) to prevent excessive memory usage */
#define alloc_max_alloc_size (usize_mebibyte * 128)

/** @brief Global heap allocator instance for general-purpose memory allocation. */
Allocator* g_alloc_heap;

/** @brief Global page allocator instance for large memory allocations. */
Allocator* g_alloc_page;

/** @brief Thread-local scratch allocator instance for temporary allocations. */
THREAD_LOCAL Allocator* g_alloc_scratch;

/**
 * @brief Initialize the global allocators.
 *
 * This function initializes the global heap and page allocators. It should be called
 * once during application startup before any memory allocation operations.
 */
void alloc_init() {
    g_alloc_heap = alloc_heap_init();
    g_alloc_page = alloc_page_init();
}

/**
 * @brief Initialize the thread-local scratch allocator.
 *
 * This function initializes the scratch allocator for the current thread. It should
 * be called once per thread that needs scratch memory allocation capabilities.
 */
void alloc_init_thread() {
    g_alloc_scratch = alloc_scratch_init();
}

/**
 * @brief Tear down the thread-local scratch allocator.
 *
 * This function cleans up the scratch allocator for the current thread and resets
 * the global scratch allocator pointer. It should be called when a thread is
 * about to exit.
 */
void alloc_teardown_thread() {
    alloc_scratch_teardown();
    g_alloc_scratch = null;
}

/**
 * @brief Allocate memory with specified size and alignment.
 *
 * This function allocates memory using the provided allocator with the specified
 * size and alignment. It performs validation on the input parameters and ensures
 * the allocator is properly initialized.
 *
 * @param allocator The allocator to use for memory allocation
 * @param size The size of memory to allocate in bytes (must be > 0)
 * @param align The alignment requirement (must be a power of 2)
 * @return A Mem structure containing the allocated memory block
 */
FORCE_INLINE Mem alloc_alloc(Allocator* allocator, const usize size, const usize align) {
    diag_assert_msg(allocator, "alloc_alloc(): Allocator is not initialized!");
    diag_assert_msg(size, "alloc_alloc(): 0 byte allocations are not valid");

    diag_assert_msg(bits_ispow2(align), "alloc_alloc(): Alignment '{}' is not a power of two", fmt_int(align));

    diag_assert_msg((size & (align - 1)) == 0, "alloc_alloc(): Size '{}' is not a multiple of the alignment '{}'", fmt_size(size), fmt_int(align));

    diag_assert_msg(size < alloc_max_alloc_size, "alloc_alloc(): Size '{}' is bigger then the maximum of '{}'", fmt_size(size), fmt_size(alloc_max_alloc_size));

    return allocator->alloc(allocator, size, align);
}

/**
 * @brief Free previously allocated memory.
 *
 * This function frees memory that was previously allocated using the same allocator.
 * It validates that the allocator is properly initialized before proceeding.
 *
 * @param allocator The allocator that was used to allocate the memory
 * @param mem The memory block to free
 */
FORCE_INLINE void alloc_free(Allocator* allocator, Mem mem) {
    diag_assert_msg(allocator, "alloc_free(): Allocator is not initialized!");

    allocator->free(allocator, mem);
}

/**
 * @brief Get the minimum allocation size supported by the allocator.
 *
 * This function returns the minimum number of bytes that can be allocated
 * by the given allocator. Allocation requests smaller than this size may
 * be rounded up to this minimum.
 *
 * @param allocator The allocator to query
 * @return The minimum allocation size in bytes
 */
FORCE_INLINE usize alloc_min_size(Allocator* allocator) {
    diag_assert_msg(allocator, "alloc_min_size(): Allocator is not initialized!");

    return allocator->minSize(allocator);
}

/**
 * @brief Get the maximum allocation size supported by the allocator.
 *
 * This function returns the maximum number of bytes that can be allocated
 * in a single allocation request by the given allocator.
 *
 * @param allocator The allocator to query
 * @return The maximum allocation size in bytes
 */
FORCE_INLINE usize alloc_max_size(Allocator *allocator) {
    diag_assert_msg(allocator, "alloc_max_size(): Allocator is not initialized!");

    return allocator->maxSize(allocator);
}