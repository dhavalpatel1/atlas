#pragma once

#ifdef ATLAS_CLANG
    #define alignof(_TYPE_) ((u32) __alignof__(_TYPE_))
#elif defined(ATLAS_GCC)
    #define alignof(_TYPE_) ((u32) __alignof__(_TYPE_))
#elif defined(ATLAS_MSVC)
    #define alignof(_TYPE_) ((u32) __alignof(_TYPE_))
#else
    _Static_assert(false, "Unsupported compiler");
#endif