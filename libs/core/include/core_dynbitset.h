#pragma once

#include "core_alloc.h"
#include "core_bitset.h"
#include "core_dynarray.h"

typedef DynArray DynBitSet;

DynBitSet dynbitset_create(Allocator* allocator, usize capacity);

void dynbitset_destroy(DynBitSet* bitset);

usize dynbitset_size(const DynBitSet* bitset);

BitSet dynbitset_view(const DynBitSet* bitset);

void dynbitset_set(DynBitSet* bitset, usize idx);

void dynbitset_or(DynBitSet* bitset, BitSet other);