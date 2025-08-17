/**
 * @file core_bits.h
 * @brief Bit manipulation functions and utilities
 *
 * This header provides functions for bit counting, shifting, and manipulation
 * operations. It includes utilities for population count, trailing zeros, and
 * bit-level conversions.
 */

#pragma once

#include "core_memory.h"
#include "core_types.h"

/** @brief Convert bits to bytes (divide by 8) */
#define bits_to_bytes(_BITS_) ((_BITS_) >> 3)

/** @brief Convert bytes to bits (multiply by 8) */
#define bytes_to_bits(_BYTES_) ((_BYTES_) * 8)

/** @brief Get the bit position within a byte (0-7) */
#define bit_in_byte(_BIT_) ((_BIT_)&0b111)

/**
 * @brief Count the number of set bits in a 32-bit value
 * @param value 32-bit value to count bits in
 * @return Number of set bits (population count)
 */
u8 bits_popcnt_32(u32 value);

/**
 * @brief Count the number of set bits in a 64-bit value
 * @param value 64-bit value to count bits in
 * @return Number of set bits (population count)
 */
u8 bits_popcnt_64(u64 value);

/**
 * @brief Generic macro for counting set bits (population count)
 * @param _VAL_ Value to count bits in (32-bit or 64-bit)
 * @return Number of set bits
 */
#define bits_popcnt(_VAL_)                  \
    _Generic((_VAL_),                       \
        u32 : bits_popcnt_32((u32)(_VAL_)), \
        u64 : bits_popcnt_64((u64)(_VAL_))  \
    )

u8 bits_ctz_32(u32);

u8 bits_ctz_64(u64);

#define bits_ctz(_VAL_)                 \
    _Generic((_VAL_),                   \
        u32: bits_ctz_32((u32)(_VAL_)), \
        u64: bits_ctz_64((u64)(_VAL_))  \
    )

u8 bits_clz_32(u32);

u8 bits_clz_64(u64);

#define bits_clz(_VAL_)                 \
    _Generic((_VAL_),                   \
        u32: bits_clz_32((u32)(_VAL_)), \
        u64: bits_clz_64((u64)(_VAL_))  \
    )

bool bits_ispow2_32(u32);

bool bits_ispow2_64(u64);

#define bits_ispow2(_VAL_)                 \
    _Generic((_VAL_),                      \
        u32: bits_ispow2_32((u32)(_VAL_)), \
        u64: bits_ispow2_64((u64)(_VAL_))  \
    )

u32 bits_nextpow2_32(u32);

u64 bits_nextpow2_64(u64);

#define bits_nextpow2(_VAL_)                 \
    _Generic((_VAL_),                        \
        u32: bits_nextpow2_32((u32)(_VAL_)), \
        u64: bits_nextpow2_64((u64)(_VAL_))  \
    )

u32 bits_hash_32(Mem);

u32 bits_padding_32(u32 val, u32 align);

u64 bits_padding_64(u64 val, u64 align);

#define bits_padding(_VAL_, _ALIGN_)                        \
    _Generic((_VAL_),                                       \
        u32: bits_padding_32((u32)(_VAL_), (u32)(_ALIGN_)), \
        u64: bits_padding_64((u64)(_VAL_), (u64)(_ALIGN_))  \
    )

u32 bits_align_32(u32 val, u32 align);

u64 bits_align_64(u64 val, u64 align);

#define bits_align(_VAL_, _ALIGN_)                        \
    _Generic((_VAL_),                                     \
        u32: bits_align_32((u32)(_VAL_), (u32)(_ALIGN_)), \
        u64: bits_align_64((u64)(_VAL_), (u64)(_ALIGN_))  \
    )

#define bits_align_ptr(_VAL_) bits_align(_VAL_, sizeof(void*))

f32 bits_u32_as_f32(u32);

u32 bits_f32_as_u32(f32);

f64 bits_u64_as_f64(u64);

u64 bits_f64_as_u64(f64);
