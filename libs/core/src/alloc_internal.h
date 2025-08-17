/**
 * @file alloc_internal.h
 * @brief Internal allocator implementation details
 *
 * This header provides the internal structure definition for allocators
 * and initialization functions for the various allocator types. This is
 * an internal header not meant for public consumption.
 */

#pragma once

#include "core_alloc.h"

/**
 * @brief Internal allocator structure definition
 *
 * This structure defines the vtable interface that all allocator
 * implementations must provide. Each allocator type implements
 * these function pointers to provide memory management services.
 */
struct sAllocator {
    /** @brief Allocate memory with specified size and alignment */
    Mem (*alloc)(Allocator* allocator, usize size, usize align);

    /** @brief Free previously allocated memory */
    void (*free)(Allocator* allocator, Mem mem);

    /** @brief Get the minimum allocation size for this allocator */
    usize (*minSize)(Allocator* allocator);

    /** @brief Get the maximum allocation size for this allocator */
    usize (*maxSize)(Allocator* allocator);
};

/**
 * @brief Initialize the global heap allocator
 * @return Pointer to the initialized heap allocator
 */
Allocator* alloc_heap_init();

/**
 * @brief Initialize the global page allocator
 * @return Pointer to the initialized page allocator
 */
Allocator* alloc_page_init();

/**
 * @brief Initialize the thread-local scratch allocator
 * @return Pointer to the initialized scratch allocator
 */
Allocator* alloc_scratch_init();

/**
 * @brief Clean up the thread-local scratch allocator
 *
 * This function should be called to properly clean up the scratch
 * allocator when a thread is terminating.
 */
void alloc_scratch_teardown();