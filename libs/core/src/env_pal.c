#if defined(ATLAS_LINUX)
    #include "env_pal_linux.c"
#elif defined(ATLAS_MSVC)
    #include "env_pal_win32.c"
#else
_Static_assert(false, "Unsupported platform")
#endif