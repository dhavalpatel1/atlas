/**
 * @file math.c
 * @brief Mathematical functions and utilities implementation.
 *
 * This file provides mathematical functions including power operations, floating-point
 * math functions, and lookup tables for efficient computation. It wraps standard C
 * library math functions and provides optimized implementations for common operations
 * like powers of 10.
 */

#include "core_array.h"
#include "core_diag.h"
#include "core_math.h"

#include <math.h>

/** @brief Lookup table for powers of 10 from 10^0 to 10^19. */
static u64 table[] = {
    u64_lit(1),                    /* 10^0  = 1 */
    u64_lit(10),                   /* 10^1  = 10 */
    u64_lit(100),                  /* 10^2  = 100 */
    u64_lit(1000),                 /* 10^3  = 1,000 */
    u64_lit(10000),                /* 10^4  = 10,000 */
    u64_lit(100000),               /* 10^5  = 100,000 */
    u64_lit(1000000),              /* 10^6  = 1,000,000 */
    u64_lit(10000000),             /* 10^7  = 10,000,000 */
    u64_lit(100000000),            /* 10^8  = 100,000,000 */
    u64_lit(1000000000),           /* 10^9  = 1,000,000,000 */
    u64_lit(10000000000),          /* 10^10 = 10,000,000,000 */
    u64_lit(100000000000),         /* 10^11 = 100,000,000,000 */
    u64_lit(1000000000000),        /* 10^12 = 1,000,000,000,000 */
    u64_lit(10000000000000),       /* 10^13 = 10,000,000,000,000 */
    u64_lit(100000000000000),      /* 10^14 = 100,000,000,000,000 */
    u64_lit(1000000000000000),     /* 10^15 = 1,000,000,000,000,000 */
    u64_lit(10000000000000000),    /* 10^16 = 10,000,000,000,000,000 */
    u64_lit(100000000000000000),   /* 10^17 = 100,000,000,000,000,000 */
    u64_lit(1000000000000000000),  /* 10^18 = 1,000,000,000,000,000,000 */
    u64_lit(10000000000000000000), /* 10^19 = 10,000,000,000,000,000,000 */
};

/**
 * @brief Get 10 raised to the specified power as a 64-bit unsigned integer.
 *
 * This function uses a lookup table for efficient computation of powers of 10.
 *
 * @param val The exponent (must be < 20)
 * @return 10^val as a u64
 */
u64 math_pow10_u64(const u8 val) {
    diag_assert(val < array_elems(table));

    return table[val];
}

FORCE_INLINE f32 math_sqrt_f32(const f32 val) {
    return sqrtf(val);
}

FORCE_INLINE f32 math_log_f32(const f32 val) {
    return logf(val);
}

FORCE_INLINE f32 math_sin_f32(const f32 val) {
    return sinf(val);
}

FORCE_INLINE f32 math_cos_f32(const f32 val) {
    return cosf(val);
}