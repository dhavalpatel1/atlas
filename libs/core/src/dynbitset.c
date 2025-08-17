/**
 * @file dynbitset.c
 * @brief Dynamic bit set implementation with automatic resizing
 *
 * This file implements a dynamic bit set data structure that automatically
 * grows to accommodate bits being set at arbitrary indices. It provides
 * efficient bit manipulation operations while maintaining a compact memory
 * representation and supporting various bitwise operations.
 */

#include "core_bits.h"
#include "core_bitset.h"
#include "core_dynbitset.h"

/**
 * @brief Calculate the number of bytes required to store a given bit index
 *
 * This function calculates the minimum number of bytes needed to store
 * a bit at the specified index. It accounts for the fact that if a bit
 * falls within a partially used byte, an additional byte is needed.
 *
 * @param bit The bit index to calculate storage for
 * @return Number of bytes required to store the bit
 */
static usize bitset_required_bytes(usize bit) {
    return bits_to_bytes(bit) + (bit_in_byte(bit) ? 1 : 0);
}

/**
 * @brief Ensure the dynamic bitset can accommodate a bit at the given index
 *
 * This function grows the dynamic bitset's storage if necessary to ensure
 * that a bit at the specified index can be stored. New bytes are initialized
 * to zero to maintain consistent behavior.
 *
 * @param dynbitset The dynamic bitset to potentially grow
 * @param bit The bit index that needs to be accommodated
 */
static void dynbitset_ensure(DynBitSet* dynbitset, usize bit) {
    const usize byte = bits_to_bytes(bit);
    if (byte >= dynbitset->size) {
        mem_set(dynarray_push(dynbitset, 1 + byte - dynbitset->size), 0);
    }
}

DynBitSet dynbitset_create(Allocator* allocator, usize capacity) {
    return dynarray_create(allocator, 1, 1, bitset_required_bytes(capacity));
}

void dynbitset_destroy(DynBitSet* dynbitset) {
    dynarray_destroy(dynbitset);
}

FORCE_INLINE usize dynbitset_size(const DynBitSet* dynbitset) {
    return bytes_to_bits(dynbitset->size);
}

BitSet dynbitset_view(const DynBitSet* dynbitset) {
    return dynarray_at(dynbitset, 0, dynbitset->size);
}

FORCE_INLINE void dynbitset_set(DynBitSet* dynbitset, usize idx) {
    dynbitset_ensure(dynbitset, idx);
    bitset_set(dynbitset_view(dynbitset), idx);
}

void dynbitset_or(DynBitSet* dynbitset, BitSet other) {
    dynbitset_ensure(dynbitset, bytes_to_bits(other.size));
    bitset_or(dynbitset_view(dynbitset), other);
}