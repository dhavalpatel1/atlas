/**
 * @file core_memory.h
 * @brief Memory management utilities and operations
 *
 * This header provides a lightweight memory abstraction with utilities for memory
 * manipulation, comparison, and iteration. The Mem structure encapsulates a pointer
 * and size pair for safe memory operations.
 */

#pragma once

#include "core_types.h"

/**
 * @brief Memory region structure containing pointer and size
 *
 * Represents a contiguous memory region with a pointer to the data and its size.
 * Used throughout the core library for safe memory operations.
 */
typedef struct {
    void* ptr;   /**< Pointer to the memory region */
    usize size;  /**< Size of the memory region in bytes */
} Mem;

/** @brief Empty memory region constant */
#define mem_empty ((Mem){0})

/**
 * @brief Create a memory region from a struct literal
 * @param _TYPE_ The struct type to create
 * @param ... Initializer list for the struct
 * @return Mem structure pointing to the created struct
 */
#define mem_struct(_TYPE_, ...)          \
    ((Mem) {                             \
        .ptr = &(_TYPE_){ __VA_ARGS__ }, \
        .size = sizeof(_TYPE_)           \
    })

/**
 * @brief Create a memory region from a pointer and size
 * @param _PTR_ Pointer to the memory region
 * @param _SIZE_ Size of the memory region in bytes
 * @return Mem structure encapsulating the pointer and size
 */
#define mem_create(_PTR_, _SIZE_) \
    ((Mem) {                      \
        .ptr = (void*)(_PTR_),    \
        .size = _SIZE_,           \
    })

/**
 * @brief Check if a memory region is valid (non-null pointer)
 * @param _MEM_ Memory region to check
 * @return true if the pointer is non-null, false otherwise
 */
#define mem_valid(_MEM_) ((_MEM_).ptr != null)

/**
 * @brief Get pointer to the beginning of a memory region
 * @param _MEM_ Memory region
 * @return Pointer to the first byte as u8*
 */
#define mem_begin(_MEM_) ((u8*)(_MEM_).ptr)

/**
 * @brief Get pointer to one past the end of a memory region
 * @param _MEM_ Memory region
 * @return Pointer to one past the last byte as u8*
 */
#define mem_end(_MEM_) ((u8*)(_MEM_).ptr + (_MEM_).size)

/**
 * @brief Get pointer to a specific byte offset in a memory region
 * @param _MEM_ Memory region
 * @param _IDX_ Byte offset from the beginning
 * @return Pointer to the byte at the specified offset as u8*
 */
#define mem_at_u8(_MEM_, _IDX_) ((u8*)(_MEM_).ptr + (_IDX_))

/**
 * @brief Cast memory region to a typed pointer
 * @param _MEM_ Memory region to cast
 * @param _TYPE_ Target type for the cast
 * @return Typed pointer to the memory region
 */
#define mem_as_t(_MEM_, _TYPE_) ((_TYPE_*)mem_as(_MEM_, sizeof(_TYPE_)))

/**
 * @brief Iterate over each byte in a memory region
 * @param _MEM_ Memory region to iterate over
 * @param _VAR_ Variable name for the current byte value
 * @param ... Code block to execute for each byte
 */
#define mem_for_u8(_MEM_, _VAR_, ...)                                                     \
{                                                                                         \
    const u8* _VAR_##_end = mem_end(_MEM_);                                               \
    for (u8* _VAR_##_itr = mem_begin(_MEM_); _VAR_##_itr != _VAR_##_end; ++_VAR_##_itr) { \
        const u8 _VAR_ = *_VAR_##_itr;                                                    \
        __VA_ARGS__                                                                       \
    }                                                                                     \
}                                                                                         \

/**
 * @brief Allocate memory on the stack
 * @param _SIZE_ Size in bytes to allocate
 * @return Mem structure pointing to stack-allocated memory
 */
#ifdef ATLAS_MSVC
#define mem_stack(_SIZE_) mem_create(_alloca(_SIZE_), _SIZE_)
#else
#define mem_stack(_SIZE_) mem_create(__builtin_alloca(_SIZE_), _SIZE_)
#endif

/**
 * @brief Set all bytes in a memory region to a specific value
 * @param mem Memory region to fill
 * @param val Value to set each byte to
 */
void mem_set(Mem mem, u8 val);

/**
 * @brief Copy memory from source to destination
 * @param dst Destination memory region
 * @param src Source memory region
 */
void mem_cpy(Mem dst, Mem src);

/**
 * @brief Move memory from source to destination (handles overlapping regions)
 * @param dst Destination memory region
 * @param src Source memory region
 */
void mem_move(Mem dst, Mem src);

/**
 * @brief Create a sub-region of a memory region
 * @param mem Source memory region
 * @param offset Byte offset from the beginning of the source
 * @param size Size of the sub-region in bytes
 * @return New Mem structure representing the sub-region
 */
Mem mem_slice(Mem mem, usize offset, usize size);

/**
 * @brief Consume bytes from the beginning of a memory region
 * @param mem Source memory region
 * @param amount Number of bytes to consume from the beginning
 * @return New Mem structure with the consumed bytes removed
 */
Mem mem_consume(Mem mem, usize amount);

/**
 * @brief Get a typed pointer from a memory region with size validation
 * @param mem Memory region to cast
 * @param size Required size for the type
 * @return Void pointer to the memory region, or null if size is insufficient
 */
void* mem_as(Mem mem, usize size);

/**
 * @brief Compare two memory regions lexicographically
 * @param a First memory region
 * @param b Second memory region
 * @return Negative if a < b, zero if equal, positive if a > b
 */
i8 mem_cmp(Mem a, Mem b);

/**
 * @brief Check if two memory regions are equal
 * @param a First memory region
 * @param b Second memory region
 * @return true if the regions have the same size and content, false otherwise
 */
bool mem_eq(Mem a, Mem b);

/**
 * @brief Check if a memory region contains a specific byte value
 * @param mem Memory region to search
 * @param byte Byte value to search for
 * @return true if the byte is found, false otherwise
 */
bool mem_contains(Mem mem, u8 byte);

/**
 * @brief Swap the contents of two memory regions of equal size
 * @param a First memory region
 * @param b Second memory region
 */
void mem_swap(Mem a, Mem b);

/**
 * @brief Swap the contents of two raw memory locations
 * @param a Pointer to first memory location
 * @param b Pointer to second memory location
 * @param size Size in bytes to swap
 */
void mem_swap_raw(void* a, void* b, const u16 size);