#pragma once

#if defined(ATLAS_CLANG) || defined(ATLAS_GCC)
    #define LIKELY(_CONDITION_) __builtin_expect(!!(_CONDITION_), true)
#else
    #define LIKELY(_CONDITION_) _CONDITION_
#endif

#if defined(ATLAS_CLANG) || defined(ATLAS_GCC)
    #define UNLIKELY(_CONDITION_) __builtin_expect(!!(_CONDITION_), false)
#else
    #define UNLIKELY(_CONDITION_) _CONDITION_
#endif

#if defined(ATLAS_CLANG)
    #define OPTIMIZE_OFF() _Pragma("clang optimize off")
#elif defined(ATLAS_GCC)
    #define OPTIMIZE_OFF() _Pragma("GCC optimize(\"-O0\")")
#elif defined(ATLAS_MSVC)
    #define OPTIMIZE_OFF() _Pragma("optimize(\"\", off)")
#else
    #define OPTIMIZE_OFF()
#endif

#define NORETURN _Noreturn

#if defined(ATLAS_CLANG) || defined(ATLAS_GCC)
    #define MAYBE_UNUSED __attribute__((unused))
#elif defined(ATLAS_MSVC)
    #define MAYBE_UNUSED __pragma(warning(suppress : 4100))
#else
    #define MAYBE_UNUSED
#endif

#if defined(ATLAS_CLANG) || defined(ATLAS_GCC)
    #define THREAD_LOCAL __thread
#elif defined(ATLAS_MSVC)
    #define THREAD_LOCAL __declspec(thread)
#else
    _Static_assert(false, "Unsupported compiler");
#endif

#if defined(ATLAS_CLANG)
    #define FORCE_INLINE __attribute__((always_inline))
#else
    #define FORCE_INLINE
#endif

#if defined(ATLAS_CLANG) || defined(ATLAS_GCC)
    #define PACKED(...) __VA_ARGS__ __attribute__((__packed__))
#elif defined(ATLAS_MSVC)
    #define PACKED(...) __pragma(pack(push, 1)) __VA_ARGS__ __pragma(pack(pop))
#else
    #define PACKED(...) __VA_ARGS__
#endif