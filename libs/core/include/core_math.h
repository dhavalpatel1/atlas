/**
 * @file core_math.h
 * @brief Mathematical functions and constants
 *
 * This header provides fundamental mathematical functions, constants, and macros
 * for numeric computations including basic arithmetic, trigonometric functions,
 * and mathematical constants.
 */

#pragma once

#include "core_types.h"

/** @brief Return the smaller of two values */
#define math_min(_A_, _B_) ((_A_) < (_B_) ? (_A_) : (_B_))
/** @brief Return the larger of two values */
#define math_max(_A_, _B_) ((_A_) > (_B_) ? (_A_) : (_B_))

/** @brief Return the sign of a value (-1, 0, or 1) */
#define math_sign(_A_) (((_A_) > 0) - ((_A_) < 0))

/** @brief Return the absolute value of a number */
#define math_abs(_A_) ((_A_) < 0 ? -(_A_) : (_A_))

/** @brief Pi constant as 32-bit float */
#define math_pi_f32 3.141592653589793238463f
/** @brief Pi constant as 64-bit float */
#define math_pi_f64 3.141592653589793238463

/**
 * @brief Calculate 10 raised to the power of n
 * @param n Exponent (must be <= 19 to avoid overflow)
 * @return 10^n as unsigned 64-bit integer
 */
u64 math_pow10_u64(u8 n);

/**
 * @brief Calculate square root of a 32-bit float
 * @param x Input value
 * @return Square root of x
 */
f32 math_sqrt_f32(f32 x);

/**
 * @brief Calculate natural logarithm of a 32-bit float
 * @param x Input value (must be positive)
 * @return Natural logarithm of x
 */
f32 math_log_f32(f32 x);

/**
 * @brief Calculate sine of an angle in radians
 * @param x Angle in radians
 * @return Sine of x
 */
f32 math_sin_f32(f32 x);

/**
 * @brief Calculate cosine of an angle in radians
 * @param x Angle in radians
 * @return Cosine of x
 */
f32 math_cos_f32(f32 x);