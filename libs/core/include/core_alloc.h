#pragma once

#include "core_memory.h"
#include "core_annotation.h"
#include "core_alignof.h"

#define alloc_bump_create_stack(_SIZE_) alloc_bump_create(mem_stack(_SIZE_))

#define alloc_alloc_t(_ALLOCATOR_, _TYPE_) \
    ((_TYPE_*)alloc_alloc((_ALLOCATOR_), sizeof(_TYPE_), alignof(_TYPE_)).ptr)

#define alloc_free_t(_ALLOCATOR_, _PTR_) \
    alloc_free((_ALLOCATOR_), mem_create((_PTR_), sizeof(*(_PTR_))))

typedef struct sAllocator Allocator;

extern Allocator* g_alloc_heap;

extern Allocator* g_alloc_page;

extern THREAD_LOCAL Allocator* g_alloc_scratch;

Allocator* alloc_bump_create(Mem mem);

Mem alloc_alloc(Allocator* allocator, usize size, usize align);

void alloc_free(Allocator* allocator, Mem mem);

usize alloc_min_size(Allocator* allocator);

usize alloc_max_size(Allocator* allocator);