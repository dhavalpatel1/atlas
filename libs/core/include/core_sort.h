#pragma once

#include "core_compare.h"
#include "core_types.h"

typedef void (*SortFunc)(u8* begin, u8* end, u16 stride, CompareFunc);

void sort_quicksort(u8* begin, u8* end, u16 stride, CompareFunc compare);

void sort_bubblesort(u8* begin, u8* end, u16 stride, CompareFunc compare);