#include "core_array.h"
#include "core_compare.h"
#include "core_diag.h"
#include "core_memory.h"
#include "core_sort.h"

static Mem quicksort_pivot(u8* begin, u8* end, u16 stride) {
    const usize elems = (end - begin) / stride;

    return mem_create(begin + elems / 2 * stride, stride);
}

static u8* quicksort_partition(u8* begin, u8* end, u16 stride, CompareFunc compare) {
    Mem pivot = mem_stack(stride);
    mem_cpy(pivot, quicksort_pivot(begin, end, stride));

    while (true) {
        while (compare(begin, pivot.ptr) < 0) {
            begin += stride;
        }

        do {
            end -= stride;
        } while (compare(end, pivot.ptr) > 0);

        if (begin >= end) {
            return begin;
        }

        mem_swap_raw(begin, end, stride);

        begin += stride;
    }
}

void sort_quicksort(u8 *begin, u8 *end, u16 stride, CompareFunc compare) {
    if ((end - begin) < (stride * 2)) {
        return;
    }

    void* partition = quicksort_partition(begin, end, stride, compare);
    
    sort_quicksort(begin, partition, stride, compare);
    
    sort_quicksort(partition, end, stride, compare);
}