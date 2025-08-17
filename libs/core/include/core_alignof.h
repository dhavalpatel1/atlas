/**
 * @file core_alignof.h
 * @brief Type alignment utilities
 *
 * This header provides cross-platform macros for determining the alignment
 * requirements of types. The alignof macro returns the alignment in bytes
 * that a type requires for proper memory layout.
 */

#pragma once

/**
 * @brief Get the alignment requirement of a type in bytes
 * @param _TYPE_ Type to get alignment for
 * @return Alignment requirement as u32
 */
#ifdef ATLAS_CLANG
    #define alignof(_TYPE_) ((u32) __alignof__(_TYPE_))
#elif defined(ATLAS_GCC)
    #define alignof(_TYPE_) ((u32) __alignof__(_TYPE_))
#elif defined(ATLAS_MSVC)
    #define alignof(_TYPE_) ((u32) __alignof(_TYPE_))
#else
    _Static_assert(false, "Unsupported compiler");
#endif