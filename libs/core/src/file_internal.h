/**
 * @file file_internal.h
 * @brief Internal file system structures and platform abstraction layer interface
 *
 * This header defines the internal file handle representation and the common
 * file structure used across different platforms. It provides platform-specific
 * type definitions for file handles and declares the PAL initialization function.
 */

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