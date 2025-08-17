/**
 * @file core_sort.h
 * @brief Sorting algorithms for arrays and data structures
 *
 * This header provides various sorting algorithms that work with generic
 * data types through comparison functions. It supports in-place sorting
 * for arrays with configurable element size and stride.
 */

#pragma once

#include "core_compare.h"
#include "core_types.h"

/** @brief Function pointer type for sorting algorithms */
typedef void (*SortFunc)(u8* begin, u8* end, u16 stride, CompareFunc);

/**
 * @brief Sort an array using the quicksort algorithm
 * @param begin Pointer to the first element
 * @param end Pointer to one past the last element
 * @param stride Size of each element in bytes
 * @param compare Comparison function for elements
 */
void sort_quicksort(u8* begin, u8* end, u16 stride, CompareFunc compare);

/**
 * @brief Sort an array using the bubble sort algorithm
 * @param begin Pointer to the first element
 * @param end Pointer to one past the last element
 * @param stride Size of each element in bytes
 * @param compare Comparison function for elements
 */
void sort_bubblesort(u8* begin, u8* end, u16 stride, CompareFunc compare);