/**
 * @file dynarray.c
 * @brief Dynamic array implementation with automatic resizing and memory management.
 *
 * This file implements a generic dynamic array data structure that automatically
 * resizes as elements are added or removed. It supports both managed (with allocator)
 * and unmanaged (over existing memory) modes, provides efficient insertion and removal
 * operations, and includes utilities for sorting and shuffling array contents.
 */

#include "core_annotation.h"
#include "core_bits.h"
#include "core_compare.h"
#include "core_diag.h"
#include "core_dynarray.h"
#include "core_math.h"
#include "core_memory.h"
#include "core_sort.h"
#include "core_shuffle.h"

DynArray dynarray_create(Allocator *allocator, const u16 stride, const u16 align, usize capacity) {
    diag_assert(stride);
    DynArray array = {
        .stride = stride,
		.align = align,
        .alloc = allocator,
    };

    if (capacity) {
        const usize capacityBytes = bits_nextpow2_64(capacity * stride);
        array.data = alloc_alloc(allocator, capacityBytes, align);

        diag_assert_msg(mem_valid(array.data), "Allocation failed");
    }

    return array;
}

DynArray dynarray_create_over(Mem memory, u16 stride) {
    diag_assert(stride);
    DynArray array = {
        .stride = stride,
		.align = 1,
        .data = memory,
    };

    return array;
}

void dynarray_destroy(DynArray *array) {
    diag_assert(array);
    if (array->alloc && LIKELY(mem_valid(array->data))) {
        // Having a allocator pointer (and a valid allocation) means we should free the backing memory.
        alloc_free(array->alloc, array->data);
    }
}

FORCE_INLINE usize dynarray_size(const DynArray* array) {
    diag_assert(array);

    return array->size;
}

/**
 * @brief Grows the dynamic array's capacity to accommodate more elements.
 *
 * This internal function reallocates the array's memory to a larger size when
 * the current capacity is insufficient. It uses power-of-2 sizing to minimize
 * the number of reallocations needed as the array grows.
 *
 * @param array The dynamic array to grow
 * @param size The minimum number of elements that need to fit
 */
static void dynarray_resize_grow(DynArray* array, const usize size) {
    diag_assert_msg(array->alloc, "DynArray without an allocator ran out of memory");

    const Mem newMem = alloc_alloc(array->alloc, bits_nextpow2_64(size * array->stride), array->align);
    diag_assert_msg(mem_valid(newMem), "Allocation failed");

    if (LIKELY(mem_valid(array->data))) {
        mem_cpy(newMem, array->data);
        alloc_free(array->alloc, array->data);
    }

    array->data = newMem;
}

FORCE_INLINE void dynarray_resize(DynArray* array, const usize size) {
    diag_assert(array);
    if (size * array->stride > array->data.size) {
        dynarray_resize_grow(array, size);
    }

    array->size = size;
}

FORCE_INLINE void dynarray_clear(DynArray* array) {
    diag_assert(array);
    array->size = 0;
}

FORCE_INLINE Mem dynarray_at(const DynArray* array, const usize idx, const usize count) {
    diag_assert(array);
    diag_assert(idx + count <= array->size);

    return mem_slice(array->data, array->stride * idx, array->stride * count);
}

FORCE_INLINE Mem dynarray_push(DynArray *array, usize count) {
    dynarray_resize(array, array->size + count);

    return mem_slice(array->data, array->stride * (array->size - count), array->stride * count);
}

FORCE_INLINE void dynarray_pop(DynArray *array, usize count) {
    diag_assert(array);
    diag_assert(count <= array->size);

    dynarray_resize(array, array->size - count);
}

void dynarray_remove(DynArray *array, const usize idx, const usize count) {
    diag_assert(array);
    diag_assert(array->size >= idx + count);

    const usize newSize = array->size - count;
    const usize entriesToMove = newSize - idx;
    if (entriesToMove) {
        const Mem dst = dynarray_at(array, idx, entriesToMove);
        const Mem src = dynarray_at(array, idx + count, entriesToMove);

        mem_move(dst, src);
    }

    array->size = newSize;
}

void dynarray_remove_unordered(DynArray *array, const  usize idx, const usize count) {
    diag_assert(array);
    diag_assert(array->size >= idx + count);

    const usize entriesToMove = math_min(count, array->size - (idx + count));
    if (entriesToMove) {
        const Mem dst = dynarray_at(array, idx, count);
        const Mem src = dynarray_at(array, array->size - entriesToMove, entriesToMove);

        mem_cpy(dst, src);
    }

    array->size -= count;
}

Mem dynarray_insert(DynArray *array, const usize idx, const usize count) {
    diag_assert(array);
    diag_assert(idx <= array->size);

    const usize entriesToMove = array->size - idx;
    dynarray_resize(array, array->size + count);
    if (entriesToMove) {
        const Mem dst = dynarray_at(array, idx + count, entriesToMove);
        const Mem src = dynarray_at(array, idx, entriesToMove);

        mem_move(dst, src);
    }

    return dynarray_at(array, idx, count);
}

void dynarray_sort(DynArray* array, CompareFunc compare) {
    const Mem mem = dynarray_at(array, 0, array->size);

    sort_bubblesort(mem_begin(mem), mem_end(mem), array->stride, compare);
}

void dynarray_shuffle(DynArray* array, Rng* rng) {
    const Mem mem = dynarray_at(array, 0, array->size);
    shuffle_fisheryates(rng, mem_begin(mem), mem_end(mem), array->stride);
}