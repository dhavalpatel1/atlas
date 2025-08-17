/**
 * @file core_sentinel.h
 * @brief Sentinel value definitions for various types
 *
 * This header provides sentinel values for different data types that can be used
 * to indicate special states like "not found", "invalid", or "uninitialized".
 * These values are typically the maximum values for integers and NaN for floats.
 */

#pragma once

#include "core_float.h"
#include "core_types.h"

/** @brief Sentinel value for 8-bit signed integers */
#define sentinel_i8 i8_max
/** @brief Sentinel value for 16-bit signed integers */
#define sentinel_i16 i16_max
/** @brief Sentinel value for 32-bit signed integers */
#define sentinel_i32 i32_max
/** @brief Sentinel value for 64-bit signed integers */
#define sentinel_i64 i64_max

/** @brief Sentinel value for 8-bit unsigned integers */
#define sentinel_u8 u8_max
/** @brief Sentinel value for 16-bit unsigned integers */
#define sentinel_u16 u16_max
/** @brief Sentinel value for 32-bit unsigned integers */
#define sentinel_u32 u32_max
/** @brief Sentinel value for 64-bit unsigned integers */
#define sentinel_u64 u64_max

/** @brief Sentinel value for size type */
#define sentinel_usize usize_max

/** @brief Sentinel value for 32-bit floats (NaN) */
#define sentinel_f32 f32_nan
/** @brief Sentinel value for 64-bit floats (NaN) */
#define sentinel_f64 f64_nan

/**
 * @brief Check if a value is a sentinel value for its type
 * @param _VAL_ Value to check
 * @return true if the value is a sentinel value, false otherwise
 */
#define sentinel_check(_VAL_) _Generic((_VAL_), \
    i8: (_VAL_) == sentinel_i8,                 \
    i16: (_VAL_) == sentinel_i16,               \
    i32: (_VAL_) == sentinel_i32,               \
    i64: (_VAL_) == sentinel_i64,               \
    u8: (_VAL_) == sentinel_u8,                 \
    u16: (_VAL_) == sentinel_u16,               \
    u32: (_VAL_) == sentinel_u32,               \
    u64: (_VAL_) == sentinel_u64,               \
    f32: float_isnan(_VAL_),                    \
    f64: float_isnan(_VAL_)                     \
)
