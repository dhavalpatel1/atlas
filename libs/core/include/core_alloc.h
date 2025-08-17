/**
 * @file core_alloc.h
 * @brief Memory allocation interfaces and allocator management
 *
 * This header provides the core memory allocation system with support for multiple
 * allocator types including heap, page, bump, and scratch allocators. It defines
 * the unified allocator interface and global allocator instances.
 */

#pragma once

#include "core_memory.h"
#include "core_annotation.h"
#include "core_alignof.h"

/**
 * @brief Create a bump allocator using stack-allocated memory
 * @param _SIZE_ Size of the stack allocation for the bump allocator
 * @return Pointer to the created bump allocator
 */
#define alloc_bump_create_stack(_SIZE_) alloc_bump_create(mem_stack(_SIZE_))

/**
 * @brief Allocate memory for a specific type with proper alignment
 * @param _ALLOCATOR_ Allocator to use for allocation
 * @param _TYPE_ Type to allocate memory for
 * @return Typed pointer to the allocated memory
 */
#define alloc_alloc_t(_ALLOCATOR_, _TYPE_) \
    ((_TYPE_*)alloc_alloc((_ALLOCATOR_), sizeof(_TYPE_), alignof(_TYPE_)).ptr)

/**
 * @brief Free memory allocated for a specific typed pointer
 * @param _ALLOCATOR_ Allocator that was used for allocation
 * @param _PTR_ Typed pointer to free
 */
#define alloc_free_t(_ALLOCATOR_, _PTR_) \
    alloc_free((_ALLOCATOR_), mem_create((_PTR_), sizeof(*(_PTR_))))

/** @brief Opaque allocator structure */
typedef struct sAllocator Allocator;

/** @brief Global heap allocator for general-purpose allocations */
extern Allocator* g_alloc_heap;

/** @brief Global page allocator for large block allocations */
extern Allocator* g_alloc_page;

/** @brief Thread-local scratch allocator for temporary allocations */
extern THREAD_LOCAL Allocator* g_alloc_scratch;

/**
 * @brief Create a bump allocator from a memory region
 * @param mem Memory region to use for the bump allocator
 * @return Pointer to the created bump allocator
 */
Allocator* alloc_bump_create(Mem mem);

/**
 * @brief Allocate aligned memory from an allocator
 * @param allocator Allocator to use
 * @param size Size in bytes to allocate
 * @param align Alignment requirement in bytes
 * @return Memory region containing the allocated memory, or empty region on failure
 */
Mem alloc_alloc(Allocator* allocator, usize size, usize align);

/**
 * @brief Free memory allocated from an allocator
 * @param allocator Allocator that was used for allocation
 * @param mem Memory region to free
 */
void alloc_free(Allocator* allocator, Mem mem);

/**
 * @brief Get the minimum allocation size for an allocator
 * @param allocator Allocator to query
 * @return Minimum allocation size in bytes
 */
usize alloc_min_size(Allocator* allocator);

/**
 * @brief Get the maximum allocation size for an allocator
 * @param allocator Allocator to query
 * @return Maximum allocation size in bytes
 */
usize alloc_max_size(Allocator* allocator);