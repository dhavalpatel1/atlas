#include "alloc_internal.h"
#include "core_alloc.h"
#include "core_annotation.h"
#include "core_bits.h"
#include "core_diag.h"
#include "core_memory.h"
#include "core_types.h"

#define alloc_scratch_heap_size (usize_mebibyte * 4)

#define alloc_scratch_max_alloc_size (usize_kibibyte * 64)

#define alloc_scratch_guard_size (usize_kibibyte * 8)

#define freed_mem_tag 0xFF

#define guard_mem_tag 0xAA

struct AllocatorScratch {
    Allocator api;
    Mem memory;
    u8* head;
};

MAYBE_UNUSED static void alloc_scratch_write_guard(struct AllocatorScratch* allocator) {
    const usize memUntilEnd = mem_end(allocator->memory) - allocator->head;
    if (memUntilEnd > alloc_scratch_guard_size) {
        mem_set(mem_create(allocator->head, alloc_scratch_guard_size), guard_mem_tag);
    } else {
        mem_set(mem_create(allocator->head, memUntilEnd), guard_mem_tag);
        mem_set(mem_create(mem_begin(allocator->memory), alloc_scratch_guard_size), guard_mem_tag);
    }
}

static Mem alloc_scratch_alloc(Allocator* allocator, const usize size, const usize align) {
    struct AllocatorScratch* allocatorScratch = (struct AllocatorScratch*)allocator;

    if (UNLIKELY(size > alloc_scratch_max_alloc_size)) {
        return mem_create(null, size);
    }

	u8* alignedHead = (u8*)bits_align((uptr)allocatorScratch->head, align);

    if (UNLIKELY(alignedHead + size > mem_end(allocatorScratch->memory))) {
        alignedHead = (u8*)bits_align((uptr)mem_begin(allocatorScratch->memory), align);
    }

    allocatorScratch->head = alignedHead + size;

    alloc_scratch_write_guard(allocatorScratch);

    return mem_create(alignedHead, size);
}

static void alloc_scratch_free(Allocator* allocator, Mem mem) {
    (void)allocator;
    (void)mem;

    diag_assert(mem_valid(mem));

    mem_set(mem, freed_mem_tag);
}

static usize alloc_scratch_min_size(Allocator* allocator) {
    (void)allocator;
    return 1;
}

static usize alloc_scratch_max_size(Allocator* allocator) {
    (void)allocator;
    return alloc_scratch_max_alloc_size;
}

static THREAD_LOCAL struct AllocatorScratch g_allocatorIntern;

Allocator* alloc_scratch_init() {
    Mem scratchPage = alloc_alloc(g_alloc_page, alloc_scratch_heap_size, sizeof(void*));
    
    g_allocatorIntern = (struct AllocatorScratch) {
        (Allocator) {
            .alloc = alloc_scratch_alloc,
            .free = alloc_scratch_free,
            .minSize = alloc_scratch_min_size,
            .maxSize = alloc_scratch_max_size,
        },
        .memory = scratchPage,
        .head = mem_begin(scratchPage),
    };

    return (Allocator*)&g_allocatorIntern;
}

void alloc_scratch_teardown() {
    alloc_free(g_alloc_page, g_allocatorIntern.memory);
}