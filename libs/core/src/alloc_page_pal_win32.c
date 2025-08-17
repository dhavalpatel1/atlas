/**
 * @file alloc_page_pal_win32.c
 * @brief Windows-specific page allocator implementation using VirtualAlloc.
 *
 * This file implements the page allocator for Windows platforms using the
 * VirtualAlloc and VirtualFree API functions. The allocator provides large
 * memory allocations aligned to page boundaries and is suitable for allocating
 * backing memory for other allocators or large data structures.
 */

#include "alloc_internal.h"
#include "core_alloc.h"
#include "core_annotation.h"
#include "core_bits.h"
#include "core_diag.h"
#include "core_memory.h"
#include "core_string.h"
#include "core_types.h"

#include <Windows.h>
#include <memoryapi.h>
#include <winnt.h>

struct AllocatorPage {
    Allocator api;
    usize pageSize;
};

static Mem alloc_page_alloc(Allocator* allocator, const usize size, const usize align) {
    const usize pageSize = ((struct AllocatorPage*)allocator)->pageSize;
	diag_assert_msg((pageSize & (align - 1)) == 0, "alloc_page_alloc(): Alignment '{}' cannot be satisfied (stronger then pageSize alignment)", fmt_int(align));
	(void)pageSize;

    void* ptr = VirtualAlloc(null, size, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);

    return mem_create(ptr, size);
}

static void alloc_page_free(Allocator* allocator, Mem mem) {
    (void)allocator;

    diag_assert(mem_valid(mem));

    const bool success = VirtualFree(mem.ptr, 0, MEM_RELEASE) == TRUE;

    if (UNLIKELY(!success)) {
        diag_crash_msg("VirtualFree() failed");
    }
}

static usize alloc_page_min_size(Allocator* allocator) {
    return ((struct AllocatorPage*)allocator)->pageSize;
}

static usize alloc_page_max_size(Allocator* allocator) {
    (void)allocator;

    return usize_max;
}

static struct AllocatorPage g_allocatorIntern;

Allocator* alloc_page_init() {
    SYSTEM_INFO si;
    GetSystemInfo(&si);
    const size_t pageSize = si.dwPageSize;

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