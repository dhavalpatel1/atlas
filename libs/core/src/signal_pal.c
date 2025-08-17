#ifdef ATLAS_LINUX
#include "signal_pal_linux.c"
#elif defined(ATLAS_WIN32)
#include "signal_pal_win32.c"
#else
_Static_assert(false, "Unsupported platform");
#endif