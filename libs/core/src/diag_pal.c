/**
 * @file diag_pal.c
 * @brief Platform abstraction layer implementation for diagnostic system
 *
 * This file provides the unified implementation of platform-specific diagnostic
 * functions by including the appropriate platform-specific implementation based
 * on compile-time platform detection.
 */

#include "diag_internal.h"

#ifdef ATLAS_LINUX
#include "diag_pal_linux.c"
#elif defined(ATLAS_WIN32)
#include "diag_pal_win32.c"
#else
_Static_assert(false, "Unsupported platform");
#endif