#include "core_diag.h"

#ifdef ATLAS_LINUX
#include "alloc_page_pal_linux.c"
#elif defined(ATLAS_WIN32)
#include "alloc_page_pal_win32.c"
#else
_Static_assert(false, "Unsupported platform");
#endif