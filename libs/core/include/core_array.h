/**
 * @file core_array.h
 * @brief Static array utilities and iteration macros
 *
 * This header provides utilities for working with static C arrays, including
 * element counting, memory region creation, and type-safe iteration.
 */

#pragma once

#include "core_memory.h"

/**
 * @brief Get the number of elements in a static array
 * @param _ARRAY_ Static array to count elements of
 * @return Number of elements in the array
 */
#define array_elems(_ARRAY_) (sizeof(_ARRAY_) / sizeof((_ARRAY_)[0]))

/**
 * @brief Create a memory region from a static array
 * @param _ARRAY_ Static array to wrap in a memory region
 * @return Mem structure containing the array data
 */
#define array_mem(_ARRAY_) mem_create((void*)(_ARRAY_), sizeof(_ARRAY_))

/**
 * @brief Iterate over elements in a static array with type safety
 * @param _ARRAY_ Static array to iterate over
 * @param _TYPE_ Type of the array elements
 * @param _VAR_ Variable name for the current element pointer
 * @param ... Code block to execute for each element
 */
#define array_for_t(_ARRAY_, _TYPE_, _VAR_, ...)              \
{                                                             \
    _TYPE_* _VAR_ = (_TYPE_*)(_ARRAY_);                                \
    _TYPE_* _VAR_##_end = (_TYPE_*)_VAR_ + array_elems(_ARRAY_); \
    for (; _VAR_ != _VAR_##_end; ++_VAR_) {                   \
        __VA_ARGS__                                           \
    }                                                         \
}
