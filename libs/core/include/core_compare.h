/**
 * @file core_compare.h
 * @brief Comparison function utilities and type-specific comparators
 *
 * This header provides comparison functions for all basic types and utilities
 * for creating custom comparison functions. It supports both normal and reverse
 * comparison functions for sorting operations.
 */

#pragma once

#include "core_types.h"

/** @brief Function pointer type for comparison functions */
typedef i8 (*CompareFunc)(const void* a, const void* b);

/**
 * @brief Declare comparison functions with a custom name
 * @param _TYPE_ Type to compare
 * @param _NAME_ Name suffix for the comparison functions
 */
#define COMPARE_DECLARE_WITH_NAME(_TYPE_, _NAME_)   \
    i8 compare_##_NAME_(const void* a, const void* b); \
    i8 compare_##_NAME_##_reverse(const void* a, const void* b);

/**
 * @brief Declare comparison functions using the type name
 * @param _TYPE_ Type to compare
 */
#define COMPARE_DECLARE(_TYPE_) COMPARE_DECLARE_WITH_NAME(_TYPE_, _TYPE_)

/** @brief Declare comparison functions for 8-bit signed integers */
COMPARE_DECLARE(i8)
/** @brief Declare comparison functions for 16-bit signed integers */
COMPARE_DECLARE(i16)
/** @brief Declare comparison functions for 32-bit signed integers */
COMPARE_DECLARE(i32)
/** @brief Declare comparison functions for 64-bit signed integers */
COMPARE_DECLARE(i64)

/** @brief Declare comparison functions for 8-bit unsigned integers */
COMPARE_DECLARE(u8)
/** @brief Declare comparison functions for 16-bit unsigned integers */
COMPARE_DECLARE(u16)
/** @brief Declare comparison functions for 32-bit unsigned integers */
COMPARE_DECLARE(u32)
/** @brief Declare comparison functions for 64-bit unsigned integers */
COMPARE_DECLARE(u64)

/** @brief Declare comparison functions for size type */
COMPARE_DECLARE(usize)

/** @brief Declare comparison functions for 32-bit floats */
COMPARE_DECLARE(f32)
/** @brief Declare comparison functions for 64-bit floats */
COMPARE_DECLARE(f64)

/** @brief Declare comparison functions for strings */
COMPARE_DECLARE_WITH_NAME(String, string)
