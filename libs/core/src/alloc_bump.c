/**
 * @file alloc_bump.c
 * @brief Bump allocator implementation for fast sequential memory allocation.
 *
 * This file implements a bump allocator (also known as a linear allocator) that
 * provides very fast allocation by simply moving a pointer forward in a pre-allocated
 * memory block. It supports limited deallocation (only the most recently allocated
 * block can be freed) and is ideal for temporary allocations with short lifetimes.
 * The allocator includes debug features like memory tagging to detect use-after-free.
 */

#include "alloc_internal.h"
#include "core_alloc.h"
#include "core_bits.h"
#include "core_diag.h"

/** @brief Memory tag value used to mark freed memory for debugging. */
#define freed_mem_tag 0xFF

/**
 * @brief Bump allocator internal structure.
 *
 * This structure contains the allocator interface and the internal state
 * for managing the bump allocation strategy.
 */
struct AllocatorBump {
    Allocator api;   /**< Base allocator interface */
    u8* head;        /**< Current allocation pointer */
    u8* tail;        /**< End of available memory */
};

/**
 * @brief Allocate memory from the bump allocator.
 *
 * This function allocates memory by moving the head pointer forward. The allocation
 * is aligned to the specified alignment requirement and fails if there's insufficient
 * space remaining in the allocator.
 *
 * @param allocator The bump allocator instance
 * @param size Size of memory to allocate in bytes
 * @param align Alignment requirement for the allocation
 * @return Mem structure containing the allocated memory, or null memory if allocation failed
 */
static Mem alloc_bump_alloc(Allocator* allocator, const usize size, const usize align) {
    struct AllocatorBump* allocatorBump = (struct AllocatorBump*)allocator;
    
	u8* alignedHead = (u8*)bits_align((uptr)allocatorBump->head, align);
	if (UNLIKELY((usize)(allocatorBump->tail - alignedHead) < size)) {
		return mem_create(null, size);
    }

    allocatorBump->head = alignedHead + size;

    return mem_create(alignedHead, size);
}

/**
 * @brief Free memory from the bump allocator.
 *
 * This function provides limited deallocation capability. It can only free the
 * most recently allocated block (LIFO order). For debugging purposes, freed
 * memory is tagged with a special pattern to help detect use-after-free bugs.
 *
 * @param allocator The bump allocator instance
 * @param mem The memory block to free (must be the most recent allocation)
 */
static void alloc_bump_free(Allocator* allocator, Mem mem) {
    diag_assert(mem_valid(mem));

    struct AllocatorBump* allocatorBump = (struct AllocatorBump*) allocator;

    // TODO: Create special compiler define to enable / disable tagging of freed memory
    mem_set(mem, freed_mem_tag); // Tag to detect use-after free

    if (mem_end(mem) == allocatorBump->head) {
        allocatorBump->head -= mem.size;
    }
}

/**
 * @brief Get the minimum allocation size for the bump allocator.
 *
 * The bump allocator can allocate any size starting from 1 byte.
 *
 * @param allocator The bump allocator instance (unused)
 * @return The minimum allocation size (1 byte)
 */
static usize alloc_bump_min_size(Allocator* allocator) {
    (void)allocator;

    return 1;
}

/**
 * @brief Get the maximum allocation size for the bump allocator.
 *
 * The maximum allocation size is the remaining space between the current
 * head pointer and the end of the allocator's memory block.
 *
 * @param allocator The bump allocator instance
 * @return The maximum number of bytes that can be allocated in one request
 */
static usize alloc_bump_max_size(Allocator* allocator) {
    struct AllocatorBump* allocatorBump = (struct AllocatorBump*)allocator;

    return allocatorBump->tail - allocatorBump->head;
}

/**
 * @brief Create a new bump allocator from a memory block.
 *
 * This function initializes a bump allocator within the provided memory block.
 * The allocator structure itself is placed at the beginning of the memory block,
 * and the remaining space is used for allocations.
 *
 * @param mem The memory block to use for the allocator (must be larger than sizeof(AllocatorBump))
 * @return Pointer to the initialized allocator, or null if the memory block is too small
 */
Allocator* alloc_bump_create(Mem mem) {
    if (mem.size <= sizeof(struct AllocatorBump)) {
        return null;
    }

    struct AllocatorBump* allocatorBump = mem_as_t(mem, struct AllocatorBump);
    allocatorBump->api = (Allocator) {
        .alloc = alloc_bump_alloc,
        .free = alloc_bump_free,
        .minSize = alloc_bump_min_size,
        .maxSize = alloc_bump_max_size
    };
    allocatorBump->head = mem_begin(mem) + sizeof(struct AllocatorBump);
    allocatorBump->tail = mem_end(mem);

    return (Allocator*)allocatorBump;
}