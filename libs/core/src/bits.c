#include "core_bits.h"
#include "core_diag.h"
#include "core_memory.h"

#include <immintrin.h>

#ifdef ATLAS_MSVC
#include "intrin.h"
#pragma intrinsic(_BitScanForward)
#pragma intrinsic(_BitScanReverse)
#endif

FORCE_INLINE u8 bits_popcnt_32(const u32 mask) {
#ifdef ATLAS_MSVC
    return __popcnt(mask);
#else
    return __builtin_popcount(mask);
#endif
}

FORCE_INLINE u8 bits_popcnt_64(const u64 mask) {
#ifdef ATLAS_MSVC
    return __popcnt64(mask);
#else
    return __builtin_popcountll(mask);
#endif
}

FORCE_INLINE u8 bits_ctz_32(const u32 mask) {
    if (mask == 0u) {
        return 32;
    }

#ifdef ATLAS_MSVC
    unsigned long result;
    _BitScanForward(&result, mask);

    return (u8)result;
#else
    return __builtin_ctz(mask);
#endif
}

FORCE_INLINE u8 bits_ctz_64(const u64 mask) {
    if (mask == 0u) {
        return 64;
    }

#ifdef ATLAS_MSVC
    unsigned long result;
    _BitScanForward64(&result, mask);

    return (u8)result;
#else
    return __builtin_ctzll(mask);
#endif
}

FORCE_INLINE u8 bits_clz_32(const u32 mask) {
    if (mask == 0u) {
        return 32u;
    }

#ifdef ATLAS_MSVC
    unsigned long result;
    _BitScanReverse(&result, mask);

    return (u8)(31u - result);
#else
    return __builtin_clz(mask);
#endif
}

FORCE_INLINE u8 bits_clz_64(const u64 mask) {
    if (mask == 0u) {
        return 64u;
    }

#ifdef ATLAS_MSVC
    unsigned long result;
    _BitScanReverse64(&result, mask);

    return (u8)(63u - result);
#else
    return __builtin_clzll(mask);
#endif
}

FORCE_INLINE bool bits_ispow2_32(const u32 val) {
    diag_assert(val != 0);

    return (val & (val - 1u)) == 0;
}

FORCE_INLINE bool bits_ispow2_64(const u64 val) {
    diag_assert(val != 0);

    return (val & (val - 1u)) == 0;
}

FORCE_INLINE u32 bits_nextpow2_32(const u32 val) {
    diag_assert(val != 0u);
    diag_assert(val <= 2147483648u);

    return 1u << (32u - bits_clz_32(val - 1u));
}

FORCE_INLINE u64 bits_nextpow2_64(const u64 val) {
    diag_assert(val != 0u);
    diag_assert(val <= u64_lit(9223372036854775808));

    return u64_lit(1) << (u64_lit(64) - bits_clz_64(val - u64_lit(1)));
}

u32 bits_hash_32(const Mem mem) {
    const u32 prime = 16777619u;
    u32 hash = 2166136261u;

    mem_for_u8(mem, byte, {
        hash ^= byte;
        hash *= prime;
    });

    hash += hash << 13u;
    hash ^= hash >> 7u;
    hash += hash << 3u;
    hash ^= hash >> 17u;
    hash += hash << 5u;

    return hash;
}

FORCE_INLINE u32 bits_padding_32(const u32 val, const u32 align) {
    diag_assert(align != 0);
    diag_assert(bits_ispow2_32(align));

    const u32 rem = val & (align - 1);

    return rem ? align - rem : 0;
}

FORCE_INLINE u64 bits_padding_64(const u64 val, const u64 align) {
    diag_assert(align != 0);
    diag_assert(bits_ispow2_64(align));

    const u64 rem = val & (align - 1);

    return rem ? align - rem : 0;
}

FORCE_INLINE u32 bits_align_32(const u32 val, const u32 align) {
    return val + bits_padding_32(val, align);
}

FORCE_INLINE u64 bits_align_64(const u64 val, const u64 align) {
    return val + bits_padding_64(val, align);
}

FORCE_INLINE f32 bits_u32_as_f32(u32 val) {
    return *(f32*)(&val);
}

FORCE_INLINE u32 bits_f32_as_u32(f32 val) {
    return *(u32*)(&val);
}

FORCE_INLINE f64 bits_u64_as_f64(u64 val) {
    return *(f64*)(&val);
}

FORCE_INLINE u64 bits_f64_as_u64(f64 val) {
    return *(u64*)(&val);
}