/**
 * @file core_bitset.h
 * @brief Fixed-size bitset operations for efficient bit manipulation
 *
 * This header provides operations on fixed-size bitsets backed by memory regions.
 * Bitsets are useful for compact storage of boolean flags and efficient set
 * operations. The implementation supports bitsets of arbitrary size backed by
 * existing memory regions, variables, or arrays.
 */

#pragma once

#include "core_memory.h"
#include "core_sentinel.h"

/** @brief Fixed-size bitset type backed by a memory region */
typedef Mem BitSet;

/**
 * @brief Create a bitset from a variable
 * @param _VAR_ Variable to use as bitset storage
 * @return BitSet backed by the variable's memory
 */
#define bitset_from_var(_VAR_)  \
    ((BitSet) {                 \
        .ptr = (void*)&(_VAR_), \
        .size = sizeof(_VAR_)   \
    })

/**
 * @brief Create a bitset from an array
 * @param _ARRAY_ Array to use as bitset storage
 * @return BitSet backed by the array's memory
 */
#define bitset_from_array(_ARRAY_) \
    ((BitSet) {                    \
        .ptr = (void*)(_ARRAY_),   \
        .size = sizeof(_ARRAY_),   \
    })

/**
 * @brief Iterate over all set bits in a bitset
 * @param _BITSET_ Bitset to iterate over
 * @param _VAR_ Variable name to use for the current bit index
 * @param ... Code block to execute for each set bit
 */
#define bitset_for(_BITSET_, _VAR_, ...)                            \
{                                                                   \
    const usize _VAR_##_size = bitset_size(_BITSET_);               \
    usize _VAR_ = 0;                                                \
    do {                                                            \
        if (sentinel_check(_VAR_ = bitset_next(_BITSET_, _VAR_))) { \
            break;                                                  \
        }                                                           \
        __VA_ARGS__                                                 \
    } while (++_VAR_ < _VAR_##_size);                               \
}

/**
 * @brief Get the total number of bits in a bitset
 * @param bitset Bitset to query
 * @return Total number of bits (memory size * 8)
 */
usize bitset_size(BitSet bitset);

/**
 * @brief Test if a specific bit is set
 * @param bitset Bitset to test
 * @param idx Bit index to test
 * @return true if the bit is set, false otherwise
 */
bool bitset_test(BitSet bitset, usize idx);

/**
 * @brief Count the number of set bits in a bitset
 * @param bitset Bitset to count
 * @return Number of bits set to 1
 */
usize bitset_count(BitSet bitset);

/**
 * @brief Check if any bits are set in a bitset
 * @param bitset Bitset to check
 * @return true if at least one bit is set, false if all bits are 0
 */
bool bitset_any(BitSet bitset);

/**
 * @brief Check if any bits are set in both bitsets (intersection test)
 * @param bitset First bitset
 * @param other Second bitset
 * @return true if any bits are set in both bitsets, false otherwise
 */
bool bitset_any_of(BitSet bitset, BitSet other);

/**
 * @brief Check if all set bits in one bitset are also set in another
 * @param bitset Bitset containing the bits to check
 * @param other Bitset to check against
 * @return true if all set bits in bitset are also set in other
 */
bool bitset_all_of(BitSet bitset, BitSet other);

/**
 * @brief Find the next set bit starting from a given index
 * @param bitset Bitset to search
 * @param idx Starting index to search from
 * @return Index of the next set bit, or sentinel value if none found
 */
usize bitset_next(BitSet bitset, usize idx);

/**
 * @brief Get the index of the nth set bit (0-based)
 * @param bitset Bitset to search
 * @param idx Ordinal index of the set bit to find
 * @return Bit index of the nth set bit, or sentinel value if not found
 */
usize bitset_index(BitSet bitset, usize idx);

/**
 * @brief Set a specific bit to 1
 * @param bitset Bitset to modify
 * @param idx Bit index to set
 */
void bitset_set(BitSet bitset, usize idx);

/**
 * @brief Clear a specific bit (set to 0)
 * @param bitset Bitset to modify
 * @param idx Bit index to clear
 */
void bitset_clear(BitSet bitset, usize idx);

/**
 * @brief Clear all bits in a bitset (set all to 0)
 * @param bitset Bitset to clear
 */
void bitset_clear_all(BitSet bitset);

/**
 * @brief Perform bitwise OR operation between two bitsets
 * @param bitset Destination bitset (modified in place)
 * @param other Source bitset to OR with
 */
void bitset_or(BitSet bitset, BitSet other);

/**
 * @brief Perform bitwise AND operation between two bitsets
 * @param bitset Destination bitset (modified in place)
 * @param other Source bitset to AND with
 */
void bitset_and(BitSet bitset, BitSet other);