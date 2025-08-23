#pragma once

#include "core_alloc.h"

struct sAllocator {
    Mem (*alloc)(Allocator* allocator, usize size, usize align);

    void (*free)(Allocator* allocator, Mem mem);

    usize (*minSize)(Allocator* allocator);

    usize (*maxSize)(Allocator* allocator);
};

Allocator* alloc_heap_init();

Allocator* alloc_page_init();

Allocator* alloc_scratch_init();

void alloc_scratch_teardown();