/**
 * @file alloc_page_pal_linux.c
 * @brief Linux platform abstraction layer for page allocator implementation
 *
 * This file implements the Linux-specific page allocator using mmap/munmap
 * system calls. It provides large memory allocations aligned to page boundaries
 * and backed directly by the operating system's virtual memory management.
 */

#include "alloc_internal.h"
#include "core_alloc.h"
#include "core_annotation.h"
#include "core_bits.h"
#include "core_diag.h"
#include "core_format.h"
#include "core_memory.h"
#include <sys/mman.h>
#include <unistd.h>

/**
 * @brief Page allocator structure for Linux
 *
 * Contains the base allocator interface and the system page size
 * for alignment and minimum allocation calculations.
 */
struct AllocatorPage {
    Allocator api;      /**< Base allocator interface */
    usize pageSize;     /**< System page size in bytes */
};

/**
 * @brief Allocate memory using Linux mmap system call
 *
 * Allocates memory directly from the operating system using mmap with
 * anonymous pages. All allocations are automatically page-aligned and
 * backed by virtual memory.
 *
 * @param allocator The page allocator instance
 * @param size Size of memory to allocate in bytes
 * @param align Alignment requirement (must not exceed page alignment)
 * @return Mem structure containing the allocated memory, or null on failure
 */
static Mem alloc_page_alloc(Allocator* allocator, const usize size, const usize align) {
    (void)align;

    const usize pageSize = ((struct AllocatorPage*)allocator)->pageSize;
	diag_assert_msg((pageSize & (align - 1)) == 0, "alloc_page_alloc(): Alignment '{}' cannot be satisfied (stronger then pageSize alignment)", fmt_int(align));
	(void)pageSize;

    void* res = mmap(null, size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);

    return mem_create(res == MAP_FAILED ? null : res, size);
}

/**
 * @brief Free memory using Linux munmap system call
 *
 * Releases memory back to the operating system using munmap. The memory
 * must have been previously allocated by this allocator.
 *
 * @param allocator The page allocator instance (unused)
 * @param mem The memory block to free
 */
static void alloc_page_free(Allocator* allocator, Mem mem) {
    (void)allocator;

    diag_assert(mem_valid(mem));

    const int res = munmap(mem.ptr, mem.size);
    if (UNLIKELY(res != 0)) {
        diag_crash_msg("munmap() failed: {}", fmt_int(res));
    }
}

/**
 * @brief Get the minimum allocation size for the page allocator
 *
 * The minimum allocation size is the system page size, as all allocations
 * are rounded up to page boundaries.
 *
 * @param allocator The page allocator instance
 * @return The system page size in bytes
 */
static usize alloc_page_min_size(Allocator* allocator) {
    return ((struct AllocatorPage*)allocator)->pageSize;
}

/**
 * @brief Get the maximum allocation size for the page allocator
 *
 * The page allocator can theoretically allocate up to the maximum size_t
 * value, limited only by available virtual address space and system memory.
 *
 * @param allocator The page allocator instance (unused)
 * @return The maximum allocation size (usize_max)
 */
static usize alloc_page_max_size(Allocator* allocator) {
    (void)allocator;

    return usize_max;
}

static struct AllocatorPage g_allocatorIntern;

Allocator* alloc_page_init() {
    const size_t pageSize = getpagesize();
    g_allocatorIntern = (struct AllocatorPage) {
        (Allocator) {
            .alloc = alloc_page_alloc,
            .free = alloc_page_free,
            .minSize = alloc_page_min_size,
            .maxSize = alloc_page_max_size,
        },
        pageSize,
    };

    return (Allocator*)&g_allocatorIntern;
}