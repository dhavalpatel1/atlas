/**
 * @file alloc_scratch.c
 * @brief Thread-local scratch allocator implementation with circular buffer and guard regions.
 *
 * This file implements a thread-local scratch allocator that provides very fast temporary
 * memory allocation using a circular buffer strategy. It allocates a large backing buffer
 * from the page allocator and uses it as a circular allocation space with guard regions
 * for corruption detection. This allocator is ideal for temporary allocations that don't
 * need explicit deallocation and have relatively short lifetimes.
 */

#include "alloc_internal.h"
#include "core_alloc.h"
#include "core_annotation.h"
#include "core_bits.h"
#include "core_diag.h"
#include "core_memory.h"
#include "core_types.h"

/** @brief Size of the scratch allocator's backing memory buffer (2 MiB). */
#define alloc_scratch_heap_size (usize_mebibyte * 2)

/** @brief Maximum size for a single scratch allocation (8 KiB). */
#define alloc_scratch_max_alloc_size (usize_kibibyte * 8)

/** @brief Size of guard regions for corruption detection (8 KiB). */
#define alloc_scratch_guard_size (usize_kibibyte * 8)

/** @brief Memory tag value used to mark freed memory for debugging. */
#define freed_mem_tag 0xFF

/** @brief Memory tag value used to mark guard regions for corruption detection. */
#define guard_mem_tag 0xAA

/**
 * @brief Scratch allocator structure with circular buffer state.
 *
 * This structure contains the allocator interface and manages the circular
 * buffer allocation state with guard regions for detecting memory corruption.
 */
struct AllocatorScratch {
    Allocator api;   /**< Base allocator interface */
    Mem memory;      /**< Backing memory buffer */
    u8* head;        /**< Current allocation pointer */
};

/**
 * @brief Write guard regions for memory corruption detection.
 *
 * This function writes guard patterns after the current head position to help
 * detect buffer overruns and memory corruption. If the guard region would extend
 * past the end of the buffer, it wraps around to the beginning.
 *
 * @param allocator The scratch allocator instance
 */
MAYBE_UNUSED static void alloc_scratch_write_guard(struct AllocatorScratch* allocator) {
    const usize memUntilEnd = mem_end(allocator->memory) - allocator->head;
    if (memUntilEnd > alloc_scratch_guard_size) {
        // Guard fits entirely after current head position
        mem_set(mem_create(allocator->head, alloc_scratch_guard_size), guard_mem_tag);
    } else {
        // Guard must wrap around - fill to end, then from beginning
        mem_set(mem_create(allocator->head, memUntilEnd), guard_mem_tag);
        mem_set(mem_create(mem_begin(allocator->memory), alloc_scratch_guard_size), guard_mem_tag);
    }
}

/**
 * @brief Allocate memory from the scratch allocator's circular buffer.
 *
 * This function allocates memory using a circular buffer strategy. It rejects
 * oversized allocations, performs alignment, and wraps around to the beginning
 * of the buffer if necessary. Guard regions are updated after each allocation.
 *
 * @param allocator The scratch allocator instance
 * @param size Size of memory to allocate in bytes (must be <= max_alloc_size)
 * @param align Alignment requirement for the allocation
 * @return Mem structure containing the allocated memory, or null memory if allocation failed
 */
static Mem alloc_scratch_alloc(Allocator* allocator, const usize size, const usize align) {
    struct AllocatorScratch* allocatorScratch = (struct AllocatorScratch*)allocator;

    // Reject oversized allocations that would dominate scratch space
    if (UNLIKELY(size > alloc_scratch_max_alloc_size)) {
        return mem_create(null, size);
    }

	u8* alignedHead = (u8*)bits_align((uptr)allocatorScratch->head, align);

    // Check if allocation fits before buffer end, wrap if necessary
    if (UNLIKELY(alignedHead + size > mem_end(allocatorScratch->memory))) {
        alignedHead = (u8*)bits_align((uptr)mem_begin(allocatorScratch->memory), align);
    }

    // Perform bump allocation: return current position, advance head
    allocatorScratch->head = alignedHead + size;

    // Update guard region for corruption detection
    alloc_scratch_write_guard(allocatorScratch);

    return mem_create(alignedHead, size);
}

/**
 * @brief Free memory from the scratch allocator (no-op with debug tagging).
 *
 * The scratch allocator doesn't support individual deallocation since it uses
 * a circular buffer strategy. This function only tags the memory with a debug
 * pattern to help detect use-after-free bugs.
 *
 * @param allocator The scratch allocator instance (unused)
 * @param mem The memory block to "free"
 */
static void alloc_scratch_free(Allocator* allocator, Mem mem) {
    (void)allocator;
    (void)mem;

    diag_assert(mem_valid(mem));

    // Fill freed memory with sentinel pattern for debugging
    mem_set(mem, freed_mem_tag);
}

/**
 * @brief Get the minimum allocation size for the scratch allocator.
 *
 * The scratch allocator can allocate any size starting from 1 byte.
 *
 * @param allocator The scratch allocator instance (unused)
 * @return The minimum allocation size (1 byte)
 */
static usize alloc_scratch_min_size(Allocator* allocator) {
    (void)allocator;
    return 1;
}

/**
 * @brief Get the maximum allocation size for the scratch allocator.
 *
 * The scratch allocator limits individual allocations to prevent any single
 * allocation from dominating the scratch space.
 *
 * @param allocator The scratch allocator instance (unused)
 * @return The maximum allocation size (8 KiB)
 */
static usize alloc_scratch_max_size(Allocator* allocator) {
    (void)allocator;
    return alloc_scratch_max_alloc_size;
}

/** @brief Thread-local instance of the scratch allocator. */
static THREAD_LOCAL struct AllocatorScratch g_allocatorIntern;

/**
 * @brief Initialize the thread-local scratch allocator.
 *
 * This function allocates a large backing buffer from the page allocator and
 * initializes the scratch allocator structure for the current thread. The
 * allocator uses circular buffer allocation with guard regions.
 *
 * @return Pointer to the initialized scratch allocator
 */
Allocator* alloc_scratch_init() {
    // Allocate backing memory from page allocator
    Mem scratchPage = alloc_alloc(g_alloc_page, alloc_scratch_heap_size, sizeof(void*));
    
    // Initialize thread-local scratch allocator structure
    g_allocatorIntern = (struct AllocatorScratch) {
        // Generic allocator interface
        (Allocator) {
            .alloc = alloc_scratch_alloc,
            .free = alloc_scratch_free,
            .minSize = alloc_scratch_min_size,
            .maxSize = alloc_scratch_max_size,
        },
        // Scratch-specific state
        .memory = scratchPage,
        .head = mem_begin(scratchPage),
    };

    // Return generic allocator interface
    return (Allocator*)&g_allocatorIntern;
}

/**
 * @brief Tear down the thread-local scratch allocator.
 *
 * This function returns the scratch allocator's backing memory to the page
 * allocator and should be called when a thread is about to exit.
 */
void alloc_scratch_teardown() {
    // Return backing memory to page allocator
    alloc_free(g_alloc_page, g_allocatorIntern.memory);
}