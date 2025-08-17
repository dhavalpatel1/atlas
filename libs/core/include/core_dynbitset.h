/**
 * @file core_dynbitset.h
 * @brief Dynamic bitset operations for growable bit collections
 *
 * This header provides operations on dynamic bitsets that can grow as needed.
 * Dynamic bitsets are backed by dynamic arrays and automatically expand when
 * bits beyond the current capacity are accessed. They combine the efficiency
 * of bitsets with the flexibility of dynamic growth.
 */

#pragma once

#include "core_alloc.h"
#include "core_bitset.h"
#include "core_dynarray.h"

/** @brief Dynamic bitset type that can grow as needed */
typedef DynArray DynBitSet;

/**
 * @brief Create a new dynamic bitset with specified initial capacity
 * @param allocator Allocator to use for memory management
 * @param capacity Initial capacity in bits
 * @return Newly created dynamic bitset
 */
DynBitSet dynbitset_create(Allocator* allocator, usize capacity);

/**
 * @brief Destroy a dynamic bitset and free its memory
 * @param bitset Pointer to the dynamic bitset to destroy
 */
void dynbitset_destroy(DynBitSet* bitset);

/**
 * @brief Get the current size of a dynamic bitset in bits
 * @param bitset Pointer to the dynamic bitset to query
 * @return Current size of the bitset in bits
 */
usize dynbitset_size(const DynBitSet* bitset);

/**
 * @brief Get a fixed bitset view of a dynamic bitset
 * @param bitset Pointer to the dynamic bitset
 * @return BitSet view of the current dynamic bitset contents
 */
BitSet dynbitset_view(const DynBitSet* bitset);

/**
 * @brief Set a specific bit in a dynamic bitset, growing if necessary
 * @param bitset Pointer to the dynamic bitset to modify
 * @param idx Bit index to set (bitset will grow if idx exceeds current size)
 */
void dynbitset_set(DynBitSet* bitset, usize idx);

/**
 * @brief Perform bitwise OR operation with a fixed bitset
 * @param bitset Pointer to the dynamic bitset to modify
 * @param other Fixed bitset to OR with
 */
void dynbitset_or(DynBitSet* bitset, BitSet other);