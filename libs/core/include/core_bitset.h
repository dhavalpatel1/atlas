#pragma once

#include "core_memory.h"
#include "core_sentinel.h"

typedef Mem BitSet;

#define bitset_from_var(_VAR_)  \
    ((BitSet) {                 \
        .ptr = (void*)&(_VAR_), \
        .size = sizeof(_VAR_)   \
    })

#define bitset_from_array(_ARRAY_) \
    ((BitSet) {                    \
        .ptr = (void*)(_ARRAY_),   \
        .size = sizeof(_ARRAY_),   \
    })

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

usize bitset_size(BitSet bitset);

bool bitset_test(BitSet bitset, usize idx);

usize bitset_count(BitSet bitset);

bool bitset_any(BitSet bitset);

bool bitset_any_of(BitSet bitset, BitSet other);

bool bitset_all_of(BitSet bitset, BitSet other);

usize bitset_next(BitSet bitset, usize idx);

usize bitset_index(BitSet bitset, usize idx);

void bitset_set(BitSet bitset, usize idx);

void bitset_clear(BitSet bitset, usize idx);

void bitset_clear_all(BitSet bitset);

void bitset_or(BitSet bitset, BitSet other);

void bitset_and(BitSet bitset, BitSet other);