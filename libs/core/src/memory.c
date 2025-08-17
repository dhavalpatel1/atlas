/**
 * @file memory.c
 * @brief Core memory manipulation utilities and operations.
 *
 * This file provides fundamental memory manipulation functions including setting,
 * copying, moving, slicing, and comparing memory blocks. It wraps standard C library
 * memory functions with additional safety checks and provides higher-level operations
 * for working with the Mem structure. All functions include bounds checking and
 * validation in debug builds.
 */

#include "core_diag.h"
#include "core_math.h"
#include "core_memory.h"

#include <string.h>

/**
 * @brief Set all bytes in a memory block to a specific value.
 *
 * This function fills the entire memory block with the specified byte value.
 * It validates that the memory block is valid before proceeding.
 *
 * @param mem The memory block to fill
 * @param val The byte value to fill with
 */
FORCE_INLINE void mem_set(const Mem mem, const u8 val) {
    diag_assert(mem_valid(mem));

    memset(mem.ptr, val, mem.size);
}

/**
 * @brief Copy memory from source to destination (non-overlapping).
 *
 * This function copies memory from the source block to the destination block.
 * The memory blocks must not overlap. It validates both memory blocks and
 * ensures the destination is large enough to hold the source data.
 *
 * @param dst The destination memory block
 * @param src The source memory block
 */
FORCE_INLINE void mem_cpy(const Mem dst, const Mem src) {
    diag_assert(mem_valid(dst));
    diag_assert(!src.size || mem_valid(src));
    diag_assert(dst.size >= src.size);

    memcpy(dst.ptr, src.ptr, src.size);
}

/**
 * @brief Move memory from source to destination (overlapping safe).
 *
 * This function copies memory from the source block to the destination block
 * and correctly handles overlapping memory regions. It validates both memory
 * blocks and ensures the destination is large enough.
 *
 * @param dst The destination memory block
 * @param src The source memory block
 */
FORCE_INLINE void mem_move(const Mem dst, const Mem src) {
    diag_assert(mem_valid(dst));
    diag_assert(mem_valid(src));
    diag_assert(dst.size >= src.size);

    memmove(dst.ptr, src.ptr, src.size);
}

/**
 * @brief Create a slice of a memory block at a specific offset.
 *
 * This function creates a new Mem structure that represents a portion of
 * the original memory block, starting at the specified offset with the
 * specified size. It validates bounds to ensure the slice is within the
 * original memory block.
 *
 * @param mem The source memory block
 * @param offset The offset from the beginning of the memory block
 * @param size The size of the slice
 * @return A new Mem structure representing the slice
 */
FORCE_INLINE Mem mem_slice(Mem mem, const usize offset, const usize size) {
    diag_assert(!size || mem_valid(mem));
    diag_assert(mem.size >= offset + size);

    return mem_create((u8*)mem.ptr + offset, size);
}

/**
 * @brief Consume a portion of memory from the beginning, returning the remainder.
 *
 * This function advances the memory pointer by the specified amount and reduces
 * the size accordingly, effectively "consuming" the beginning of the memory block.
 * It validates that there's enough memory to consume.
 *
 * @param mem The source memory block
 * @param amount The number of bytes to consume from the beginning
 * @return A new Mem structure representing the remaining memory
 */
FORCE_INLINE Mem mem_consume(Mem mem, usize amount) {
    diag_assert(mem.size >= amount);

    return (Mem) {
        .ptr = (u8*)mem.ptr + amount,
        .size = mem.size - amount,
    };
}

/**
 * @brief Get a pointer to memory with size validation.
 *
 * This function returns the memory pointer after validating that the memory
 * block is valid and contains at least the specified number of bytes.
 *
 * @param mem The memory block
 * @param size The minimum required size
 * @return Pointer to the memory
 */
FORCE_INLINE void* mem_as(Mem mem, const usize size) {
    diag_assert(mem_valid(mem));
    diag_assert(mem.size >= size);

    return mem.ptr;
}

/**
 * @brief Compare two memory blocks lexicographically.
 *
 * This function compares two memory blocks byte by byte and returns the sign
 * of the comparison result. It compares up to the size of the smaller block.
 *
 * @param a The first memory block
 * @param b The second memory block
 * @return -1 if a < b, 0 if equal (up to min size), 1 if a > b
 */
FORCE_INLINE i8 mem_cmp(const Mem a, const Mem b) {
    diag_assert(mem_valid(a));
    diag_assert(mem_valid(b));

    return math_sign(memcmp(a.ptr, b.ptr, math_min(a.size, b.size)));
}

/**
 * @brief Check if two memory blocks are equal in size and content.
 *
 * This function compares two memory blocks for equality, checking both
 * size and content. Both blocks must have the same size and identical
 * byte content to be considered equal.
 *
 * @param a The first memory block
 * @param b The second memory block
 * @return true if the memory blocks are equal, false otherwise
 */
FORCE_INLINE bool mem_eq(Mem a, Mem b) {
    diag_assert(!a.size || mem_valid(a));
    diag_assert(!b.size || mem_valid(b));

    return a.size == b.size && memcmp(a.ptr, b.ptr, a.size) == 0;
}

/**
 * @brief Check if a memory block contains a specific byte value.
 *
 * This function searches through the memory block to determine if it
 * contains at least one occurrence of the specified byte value.
 *
 * @param mem The memory block to search
 * @param byte The byte value to search for
 * @return true if the byte is found, false otherwise
 */
bool mem_contains(Mem mem, const u8 byte) {
    mem_for_u8(mem, b, {
        if (b == byte) {
            return true;
        }
    });

    return false;
}

/**
 * @brief Swap the contents of two memory blocks of equal size.
 *
 * This function exchanges the contents of two memory blocks that must
 * be the same size. It uses a stack-allocated buffer for the swap operation.
 *
 * @param a The first memory block
 * @param b The second memory block (must be same size as a)
 */
void mem_swap(Mem a, Mem b) {
    diag_assert(mem_valid(a));
    diag_assert(mem_valid(b));
    diag_assert(a.size == b.size);

    mem_swap_raw(a.ptr, b.ptr, (u16)a.size);
}

/**
 * @brief Swap the contents of two raw memory regions.
 *
 * This function exchanges the contents of two raw memory regions using
 * a stack-allocated buffer. The size is limited to 1024 bytes to ensure
 * the stack buffer doesn't grow too large.
 *
 * @param a Pointer to the first memory region
 * @param b Pointer to the second memory region
 * @param size The size of both memory regions (must be <= 1024)
 */
void mem_swap_raw(void *a, void *b, const u16 size) {
    diag_assert(size <= 1024);

    Mem buffer = mem_stack(size);
    memcpy(buffer.ptr, a, size);
    memcpy(a, b, size);
    memcpy(b, buffer.ptr, size);
}