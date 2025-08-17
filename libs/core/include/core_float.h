/**
 * @file core_float.h
 * @brief Floating-point constants and utilities
 *
 * This header provides floating-point constants including special values
 * (NaN, infinity), limits (min, max, epsilon), and utilities for working
 * with IEEE 754 floating-point numbers.
 */

#pragma once

/** @brief Number of mantissa bits in 32-bit float */
#define f32_mantissa_bits 23
/** @brief Number of mantissa bits in 64-bit float */
#define f64_mantissa_bits 52

/** @brief Maximum mantissa value for 32-bit float */
#define f32_mantissa_max ((((u32)1) << f32_mantissa_bits) - 1)
/** @brief Maximum mantissa value for 64-bit float */
#define f64_mantissa_max ((((u64)1) << f64_mantissa_bits) - 1)

/** @brief Maximum base-10 exponent for 32-bit float */
#define f32_exponent_max 38
/** @brief Maximum base-10 exponent for 64-bit float */
#define f64_exponent_max 308

/** @brief Not-a-Number (NaN) value for 32-bit float */
#define f32_nan bits_u32_as_f32(0x7fc00000u)
/** @brief Not-a-Number (NaN) value for 64-bit float */
#define f64_nan bits_u64_as_f64(u64_lit(0x7ff8000000000000))

/** @brief Positive infinity value for 32-bit float */
#define f32_inf bits_u32_as_f32(0x7f800000u)
/** @brief Positive infinity value for 64-bit float */
#define f64_inf bits_u64_as_f64(u64_lit(0x7ff0000000000000))

/** @brief Minimum (most negative) finite value for 32-bit float */
#define f32_min bits_u32_as_f32(0xff7fffffu)
/** @brief Minimum (most negative) finite value for 64-bit float */
#define f64_min bits_u64_as_f64(u64_lit(0xffefffffffffffff))

/** @brief Maximum finite value for 32-bit float */
#define f32_max bits_u32_as_f32(0x7f7fffffu)
/** @brief Maximum finite value for 64-bit float */
#define f64_max bits_u64_as_f64(u64_lit(0x7fefffffffffffff))

/** @brief Smallest positive subnormal value for 32-bit float */
#define f32_epsilon 1.401298E-45
/** @brief Smallest positive subnormal value for 64-bit float */
#define f64_epsilon 4.94065645841247E-324

/**
 * @brief Check if a floating-point value is NaN
 * @param _VAL_ Value to check
 * @return true if the value is NaN, false otherwise
 */
#define float_isnan(_VAL_) ((_VAL_) != (_VAL_))

/**
 * @brief Check if a floating-point value is infinite
 * @param _VAL_ Value to check
 * @return true if the value is infinite, false otherwise
 */
#define float_isinf(_VAL_) ((_VAL_) != 0.0 && (_VAL_) * 2 == (_VAL_))