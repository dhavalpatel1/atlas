
#pragma once

#include "core_alignof.h"
#include "core_compare.h"
#include "core_memory.h"
#include "core_types.h"

typedef struct sAllocator Allocator;

typedef struct sRng Rng;

typedef struct {
    Mem data;
    Allocator* alloc;
    usize size;
    u16 stride;
	u16 align;
} DynArray;

#define dynarray_create_t(_ALLOCATOR_, _TYPE_, _CAPACITY_) \
    dynarray_create((_ALLOCATOR_), (u16)sizeof(_TYPE_), (u16)alignof(_TYPE_), _CAPACITY_)

#define dynarray_create_over_t(_MEM_, _TYPE_) dynarray_create_over((_MEM_), (u16)sizeof(_TYPE_))

#define dynarray_at_t(_ARRAY_, _IDX_, _TYPE_) mem_as_t(dynarray_at(_ARRAY_, _IDX_, 1), _TYPE_)

#define dynarray_push_t(_ARRAY_, _TYPE_) mem_as_t(dynarray_push(_ARRAY_, 1), _TYPE_)

#define dynarray_for_t(_ARRAY_, _TYPE_, _VAR_, ...)                            \
{                                                                              \
    DynArray* _VAR_##_array = (_ARRAY_);                                       \
    for (usize _VAR_##_i = 0; _VAR_##_i != _VAR_##_array->size; ++_VAR_##_i) { \
        _TYPE_ * _VAR_ = dynarray_at_t(_VAR_##_array, _VAR_##_i, _TYPE_);      \
        __VA_ARGS__                                                            \
    }                                                                          \
}

DynArray dynarray_create(Allocator* allocator, u16 stride, u16 align, usize capacity);

DynArray dynarray_create_over(Mem memory, u16 stride);

void dynarray_destroy(DynArray* array);

usize dynarray_size(const DynArray* array);

void dynarray_resize(DynArray* array, usize size);

void dynarray_clear(DynArray* array);

Mem dynarray_at(const DynArray* array, usize idx, usize count);

Mem dynarray_push(DynArray* array, usize count);

void dynarray_pop(DynArray* array, usize count);

void dynarray_remove(DynArray* array, usize idx, usize count);

void dynarray_remove_unordered(DynArray* array, usize idx, usize count);

Mem dynarray_insert(DynArray* array, usize idx, usize count);

void dynarray_sort(DynArray* array, CompareFunc compare);

void dynarray_shuffle(DynArray*, Rng*);