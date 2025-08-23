#pragma once

#include "core_file.h"

#ifdef ATLAS_LINUX
typedef int FileHandle;
#elif defined(ATLAS_WIN32)
typedef void* FileHandle;
#else
_Static_assert(false, "Unsupported platform");
#endif

struct sFile {
    FileHandle handle;
    FileAccessFlags access;
    void* mapping;
    Allocator* allocator;
};

void file_pal_init();