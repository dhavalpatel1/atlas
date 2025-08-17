/**
 * @file core_annotation.h
 * @brief Compiler-specific annotations and optimization directives
 *
 * This header provides cross-platform macros for compiler-specific annotations
 * including branch prediction hints, optimization control, debugging utilities,
 * and function/variable attributes.
 */

#pragma once

/**
 * @brief Mark a condition as likely to be true for branch prediction optimization
 * @param _CONDITION_ Condition expression to evaluate
 * @return The condition value, with compiler hint that it's likely true
 */
#if defined(ATLAS_CLANG) || defined(ATLAS_GCC)
    #define LIKELY(_CONDITION_) __builtin_expect(!!(_CONDITION_), true)
#else
    #define LIKELY(_CONDITION_) _CONDITION_
#endif

/**
 * @brief Mark a condition as unlikely to be true for branch prediction optimization
 * @param _CONDITION_ Condition expression to evaluate
 * @return The condition value, with compiler hint that it's likely false
 */
#if defined(ATLAS_CLANG) || defined(ATLAS_GCC)
    #define UNLIKELY(_CONDITION_) __builtin_expect(!!(_CONDITION_), false)
#else
    #define UNLIKELY(_CONDITION_) _CONDITION_
#endif

/**
 * @brief Disable compiler optimizations for debugging purposes
 */
#if defined(ATLAS_CLANG)
    #define OPTIMIZE_OFF() _Pragma("clang optimize off")
#elif defined(ATLAS_GCC)
    #define OPTIMIZE_OFF() _Pragma("GCC optimize(\"-O0\")")
#elif defined(ATLAS_MSVC)
    #define OPTIMIZE_OFF() _Pragma("optimize(\"\", off)")
#else
    #define OPTIMIZE_OFF()
#endif

/**
 * @brief Trigger a debugger breakpoint or trap
 */
#if defined(ATLAS_LINUX)
    #define DEBUG_BREAK() __builtin_trap()
#elif defined(ATLAS_WIN32)
    #define DEBUG_BREAK() __debugbreak()
#else
    #define DEBUG_BREAK()
#endif

/** @brief Mark a function as never returning */
#define NORETURN _Noreturn

/**
 * @brief Mark a variable or parameter as potentially unused to suppress warnings
 */
#if defined(ATLAS_CLANG) || defined(ATLAS_GCC)
    #define MAYBE_UNUSED __attribute__((unused))
#elif defined(ATLAS_MSVC)
    #define MAYBE_UNUSED __pragma(warning(suppress : 4100))
#else
    #define MAYBE_UNUSED
#endif

/**
 * @brief Mark a variable as thread-local storage
 */
#if defined(ATLAS_CLANG) || defined(ATLAS_GCC)
    #define THREAD_LOCAL __thread
#elif defined(ATLAS_MSVC)
    #define THREAD_LOCAL __declspec(thread)
#else
    _Static_assert(false, "Unsupported compiler");
#endif

/**
 * @brief Force a function to be inlined by the compiler
 */
#if defined(ATLAS_CLANG)
    #define FORCE_INLINE __attribute__((always_inline))
#else
    #define FORCE_INLINE
#endif

/**
 * @brief Pack a structure with no padding between members
 * @param ... Structure definition to pack
 */
#if defined(ATLAS_CLANG) || defined(ATLAS_GCC)
    #define PACKED(...) __VA_ARGS__ __attribute__((__packed__))
#elif defined(ATLAS_MSVC)
    #define PACKED(...) __pragma(pack(push, 1)) __VA_ARGS__ __pragma(pack(pop))
#else
    #define PACKED(...) __VA_ARGS__
#endif