/**
 * @file core_types.h
 * @brief Core type definitions and fundamental constants
 *
 * This header provides standardized type aliases for integers, floating-point numbers,
 * boolean values, and size types, along with min/max constants and binary size units.
 * These types form the foundation for all other core library functionality.
 */

#pragma once

#include <stddef.h>
#include <stdint.h>

/** @brief 8-bit signed integer type */
typedef int8_t i8;
/** @brief 16-bit signed integer type */
typedef int16_t i16;
/** @brief 32-bit signed integer type */
typedef int32_t i32;
/** @brief 64-bit signed integer type */
typedef int64_t i64;

/** @brief 8-bit unsigned integer type */
typedef uint8_t u8;
/** @brief 16-bit unsigned integer type */
typedef uint16_t u16;
/** @brief 32-bit unsigned integer type */
typedef uint32_t u32;
/** @brief 64-bit unsigned integer type */
typedef uint64_t u64;

/** @brief Unsigned size type for array indices and memory sizes */
typedef size_t usize;
/** @brief Signed pointer-sized integer type */
typedef intptr_t iptr;
/** @brief Unsigned pointer-sized integer type */
typedef uintptr_t uptr;

/** @brief 32-bit floating-point type */
typedef float f32;
/** @brief 64-bit floating-point type */
typedef double f64;

/** @brief Boolean type using 8-bit unsigned integer */
typedef u8 bool;
/** @brief Boolean true value */
#define true 1
/** @brief Boolean false value */
#define false 0

/** @brief Null pointer constant */
#define null 0

/** @brief Create a 32-bit unsigned integer literal with proper type */
#define u32_lit(_LITERAL_) UINT32_C(_LITERAL_)
/** @brief Create a 32-bit signed integer literal with proper type */
#define i32_lit(_LITERAL_) INT32_C(_LITERAL_)
/** @brief Create a 64-bit unsigned integer literal with proper type */
#define u64_lit(_LITERAL_) UINT64_C(_LITERAL_)
/** @brief Create a 64-bit signed integer literal with proper type */
#define i64_lit(_LITERAL_) INT64_C(_LITERAL_)

/** @brief Minimum value for 8-bit signed integer */
#define i8_min INT8_MIN
/** @brief Minimum value for 16-bit signed integer */
#define i16_min INT16_MIN
/** @brief Minimum value for 32-bit signed integer */
#define i32_min INT32_MIN
/** @brief Minimum value for 64-bit signed integer */
#define i64_min INT64_MIN
/** @brief Minimum value for pointer-sized signed integer */
#define iptr_min INTPTR_MIN

/** @brief Maximum value for 8-bit signed integer */
#define i8_max INT8_MAX
/** @brief Maximum value for 16-bit signed integer */
#define i16_max INT16_MAX
/** @brief Maximum value for 32-bit signed integer */
#define i32_max INT32_MAX
/** @brief Maximum value for 64-bit signed integer */
#define i64_max INT64_MAX
/** @brief Maximum value for pointer-sized signed integer */
#define iptr_max INTPTR_MAX

/** @brief Maximum value for 8-bit unsigned integer */
#define u8_max UINT8_MAX
/** @brief Maximum value for 16-bit unsigned integer */
#define u16_max UINT16_MAX
/** @brief Maximum value for 32-bit unsigned integer */
#define u32_max UINT32_MAX
/** @brief Maximum value for 64-bit unsigned integer */
#define u64_max UINT64_MAX
/** @brief Maximum value for size type */
#define usize_max SIZE_MAX
/** @brief Maximum value for pointer-sized signed integer */
#define iptr_max INTPTR_MAX
/** @brief Maximum value for pointer-sized unsigned integer */
#define uptr_max UINTPTR_MAX

/** @brief Size constant for one byte */
#define usize_byte ((usize)1)
/** @brief Size constant for one kibibyte (1024 bytes) */
#define usize_kibibyte (usize_byte * 1024)
/** @brief Size constant for one mebibyte (1024 kibibytes) */
#define usize_mebibyte (usize_kibibyte * 1024)
/** @brief Size constant for one gibibyte (1024 mebibytes) */
#define usize_gibibyte (usize_mebibyte * 1024)
/** @brief Size constant for one tebibyte (1024 gibibytes) */
#define usize_tebibyte (usize_gibibyte * 1024)
/** @brief Size constant for one pebibyte (1024 tebibytes) */
#define usize_pebibyte (usize_tebibyte * 1024)