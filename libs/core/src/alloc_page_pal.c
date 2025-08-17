/**
 * @file alloc_page_pal.c
 * @brief Platform abstraction layer for page allocator implementation.
 *
 * This file provides platform-specific inclusion of page allocator implementations.
 * It selects the appropriate platform-specific implementation (Linux or Windows)
 * based on compile-time platform detection macros. The page allocator provides
 * large memory allocations using system virtual memory functions.
 */

#include "core_diag.h"

#ifdef ATLAS_LINUX
#include "alloc_page_pal_linux.c"
#elif defined(ATLAS_WIN32)
#include "alloc_page_pal_win32.c"
#else
_Static_assert(false, "Unsupported platform");
#endif