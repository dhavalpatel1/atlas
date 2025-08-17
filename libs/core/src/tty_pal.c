#ifdef ATLAS_LINUX
#include "tty_pal_linux.c"
#elif defined(ATLAS_WIN32)
#include "tty_pal_win32.c"
#else
_Static_assert(false, "Unsupported platform");
#endif