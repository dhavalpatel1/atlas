/**
 * @file sort_bubblesort.c
 * @brief Bubble sort algorithm implementation for generic data types
 *
 * This file implements the bubble sort algorithm, a simple comparison-based
 * sorting algorithm. While not efficient for large datasets (O(n²) time complexity),
 * it provides a stable sort and is useful for educational purposes and small arrays.
 */

#include "core_array.h"
#include "core_diag.h"
#include "core_sort.h"

void sort_bubblesort(u8* begin, u8* end, u16 stride, CompareFunc compare) {
    // Calculate the number of elements in the array
    usize len = (end - begin) / stride;
    
    // Continue until no swaps are made in a complete pass
    while (len) {
        usize newLen = 0;  // Track position of last swap
        
        // Perform one pass through the unsorted portion
        for (usize i = 1; i != len; ++i) {
            // Calculate pointers to adjacent elements
            u8* a = begin + (i - 1) * stride;
            u8* b = begin + i * stride;
            
            // Compare adjacent elements
            if (compare(a, b) > 0) {
                // Swap elements if they are out of order
                mem_swap_raw(a, b, stride);
                
                // Record position of this swap - elements after this
                // position will be checked in the next pass
                newLen = i;
            }
        }
        
        // Optimization: Only check up to the last swap position next time
        // If newLen is 0, no swaps occurred and array is sorted
        len = newLen;
    }
}