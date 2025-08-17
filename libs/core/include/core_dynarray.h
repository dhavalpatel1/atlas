/**
 * @file core_dynarray.h
 * @brief Dynamic array implementation with automatic memory management
 *
 * This header provides a type-safe dynamic array implementation that can grow
 * and shrink at runtime. The array manages its own memory allocation and provides
 * utilities for element access, insertion, removal, and iteration.
 */

#pragma once

#include "core_alignof.h"
#include "core_compare.h"
#include "core_memory.h"
#include "core_types.h"

/** @brief Forward declaration of allocator structure */
typedef struct sAllocator Allocator;

/** @brief Forward declaration of random number generator */
typedef struct sRng Rng;

/**
 * @brief Dynamic array structure for type-safe resizable arrays
 *
 * Contains the data buffer, allocator reference, size information, and
 * element metadata for proper memory management and type safety.
 */
typedef struct {
    Mem data;           /**< Memory region containing the array data */
    Allocator* alloc;   /**< Allocator used for memory management */
    usize size;         /**< Current number of elements in the array */
    u16 stride;         /**< Size of each element in bytes */
	u16 align;          /**< Alignment requirement for elements */
} DynArray;

/**
 * @brief Create a typed dynamic array with specified capacity
 * @param _ALLOCATOR_ Allocator to use for memory management
 * @param _TYPE_ Type of elements to store
 * @param _CAPACITY_ Initial capacity of the array
 * @return DynArray configured for the specified type
 */
#define dynarray_create_t(_ALLOCATOR_, _TYPE_, _CAPACITY_) \
    dynarray_create((_ALLOCATOR_), (u16)sizeof(_TYPE_), (u16)alignof(_TYPE_), _CAPACITY_)

/**
 * @brief Create a typed dynamic array over existing memory
 * @param _MEM_ Memory region to use for the array
 * @param _TYPE_ Type of elements to store
 * @return DynArray configured for the specified type
 */
#define dynarray_create_over_t(_MEM_, _TYPE_) dynarray_create_over((_MEM_), (u16)sizeof(_TYPE_))

/**
 * @brief Get a typed pointer to an element at a specific index
 * @param _ARRAY_ Dynamic array to access
 * @param _IDX_ Index of the element
 * @param _TYPE_ Type to cast the element to
 * @return Typed pointer to the element
 */
#define dynarray_at_t(_ARRAY_, _IDX_, _TYPE_) mem_as_t(dynarray_at(_ARRAY_, _IDX_, 1), _TYPE_)

/**
 * @brief Push a new element and get a typed pointer to it
 * @param _ARRAY_ Dynamic array to push to
 * @param _TYPE_ Type of the element to push
 * @return Typed pointer to the newly added element
 */
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