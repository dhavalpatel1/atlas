/**
 * @file sort_quicksort.c
 * @brief Quick sort algorithm implementation for generic data types
 *
 * This file implements the quicksort algorithm which provides O(n log n) average
 * case performance for sorting arrays of arbitrary data types. The implementation
 * uses the median-of-three pivot selection strategy and handles generic elements
 * through stride-based pointer arithmetic.
 */

#include "core_array.h"
#include "core_compare.h"
#include "core_diag.h"
#include "core_memory.h"
#include "core_sort.h"

/**
 * @brief Selects the pivot element for partitioning
 *
 * This function implements a simple pivot selection strategy by choosing
 * the middle element of the array. A more sophisticated implementation
 * might use median-of-three to avoid worst-case performance on sorted data.
 *
 * @param begin Pointer to the first element in the range
 * @param end Pointer past the last element in the range
 * @param stride Size of each element in bytes
 * @return Memory region containing the pivot element
 */
static Mem quicksort_pivot(u8* begin, u8* end, u16 stride) {
    const usize elems = (end - begin) / stride;

    // Choose the middle element as pivot
    return mem_create(begin + elems / 2 * stride, stride);
}

/**
 * @brief Partitions the array around a pivot element
 *
 * This function implements the Hoare partition scheme, which rearranges the array
 * so that all elements less than the pivot come before all elements greater than
 * or equal to the pivot. The function uses two pointers that move towards each
 * other, swapping elements that are on the wrong side of the partition.
 *
 * @param begin Pointer to the first element in the range to partition
 * @param end Pointer past the last element in the range to partition
 * @param stride Size of each element in bytes
 * @param compare Comparison function for ordering elements
 * @return Pointer to the partition point (first element >= pivot)
 */
static u8* quicksort_partition(u8* begin, u8* end, u16 stride, CompareFunc compare) {
    // Copy pivot to stack to avoid issues if pivot is moved during partitioning
    Mem pivot = mem_stack(stride);
    mem_cpy(pivot, quicksort_pivot(begin, end, stride));

    while (true) {
        // Move begin pointer right while elements are less than pivot
        while (compare(begin, pivot.ptr) < 0) {
            begin += stride;
        }

        // Move end pointer left while elements are greater than pivot
        do {
            end -= stride;
        } while (compare(end, pivot.ptr) > 0);

        // If pointers have crossed, partitioning is complete
        if (begin >= end) {
            return begin;
        }

        // Swap elements that are on the wrong side of the partition
        mem_swap_raw(begin, end, stride);

        // Move begin pointer past the swapped element
        begin += stride;
    }
}

void sort_quicksort(u8 *begin, u8 *end, u16 stride, CompareFunc compare) {
    // Base case: arrays with fewer than 2 elements are already sorted
    if ((end - begin) < (stride * 2)) {
        return;
    }

    // Partition array around pivot and get partition point
    // After partitioning: [elements < pivot][elements >= pivot]
    void* partition = quicksort_partition(begin, end, stride, compare);
    
    // Recursively sort left partition (elements < pivot)
    sort_quicksort(begin, partition, stride, compare);
    
    // Recursively sort right partition (elements >= pivot)  
    sort_quicksort(partition, end, stride, compare);
}