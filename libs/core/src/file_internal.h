#pragma once

#include "core_file.h"

#if defined(ATLAS_LINUX)
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

FileResult file_pal_create_dir_single_sync(String path);
