/**
 * @file file_pal_linux.c
 * @brief Linux platform abstraction layer for file operations
 *
 * This file implements the Linux-specific file system operations including
 * file creation, reading, writing, deletion, and directory management.
 * It translates between the cross-platform file API and Linux system calls.
 */

#include "core_alloc.h"
#include "core_diag.h"
#include "core_dynstring.h"
#include "core_file.h"
#include "core_format.h"
#include "core_memory.h"
#include "core_string.h"
#include "core_types.h"
#include "file_internal.h"
#include "linux/limits.h"
#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>

/** @brief Standard input file handle */
File* g_file_stdin = &(File){ .handle = 0 };

/** @brief Standard output file handle */
File* g_file_stdout = &(File){ .handle = 1 };

/** @brief Standard error file handle */
File* g_file_stderr = &(File){ .handle = 2 };

/**
 * @brief Convert Linux errno values to cross-platform FileResult codes
 *
 * This function maps Linux-specific error codes from errno to the
 * platform-independent FileResult enumeration for consistent error
 * handling across different operating systems.
 *
 * @return FileResult code corresponding to the current errno value
 */
static FileResult fileresult_from_errno() {
    switch (errno) {
        case EACCES:
        case EPERM:
        case EROFS: {
            return FileResult_NoAccess;
        }

        case ETXTBSY: {
            return FileResult_Locked;
        }

        case EDQUOT:
        case ENOSPC: {
            return FileResult_DiskFull;
        }

        case ENOENT: {
            return FileResult_NotFound;
        }

        case EMFILE:
        case ENFILE: {
            return FileResult_TooManyOpenFiles;
        }

        case ENAMETOOLONG: {
            return FileResult_PathTooLong;
        }

        case EEXIST: {
            return FileResult_AlreadyExists;
        }

        case EINVAL: {
            return FileResult_InvalidFilename;
        }

        case EISDIR: {
            return FileResult_IsDirectory;
        }
    }

    return FileResult_UnknownError;
}

void file_pal_init() {}

FileResult file_create(Allocator* allocator, String path, FileMode mode, FileAccessFlags access, File** file) {
    if (path.size >= PATH_MAX) {
        return FileResult_PathTooLong;
    }

    Mem pathBuffer = mem_stack(PATH_MAX);
    mem_cpy(pathBuffer, path);
    *mem_at_u8(pathBuffer, path.size) = '\0';

    int flags = O_NOCTTY;

    switch (mode) {
        case FileMode_Open: {
            /* No additional flags for simple open */
        } break;

        case FileMode_Append: {
            flags |= O_CREAT | O_APPEND;
        } break;

        case FileMode_Create: {
            flags |= O_CREAT | O_TRUNC;
        } break;

        default:
        diag_assert_fail("Invalid FileMode: {}", fmt_int(flags));
    }

    if (access & FileAccess_Read) {
        flags |= O_RDONLY;
    }

    if (access & FileAccess_Write) {
        flags |= O_WRONLY;
    }

    const int newFilePerms = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;
    const int fd = open(pathBuffer.ptr, flags, newFilePerms);
    if (fd < 0) {
        return fileresult_from_errno();
    }

    *file = alloc_alloc_t(allocator, File);
    **file = (File) {
        .handle = fd,
        .allocator = allocator,
    };

    return FileResult_Success;
}

FileResult file_temp(Allocator* allocator, File** file) {
    String nameTemplate = string_lit("atlas_tmp_XXXXXX");
    Mem nameBuffer = mem_stack(PATH_MAX);
    mem_cpy(nameBuffer, nameTemplate);
    *mem_at_u8(nameBuffer, nameTemplate.size) = '\0';

    int fd = mkstemp(nameBuffer.ptr);
    if (fd < 0) {
        return fileresult_from_errno();
    }

    unlink(nameBuffer.ptr);

    *file = alloc_alloc_t(allocator, File);
    **file = (File) {
        .handle = fd,
        .allocator = allocator
    };

    return FileResult_Success;
}

void file_destroy(File* file) {
    diag_assert_msg(file->allocator, "Invalid file");
    close(file->handle);
    alloc_free_t(file->allocator, file);
}

FileResult file_write_sync(File* file, const String data) {
    diag_assert(file);

    for (u8* itr = mem_begin(data); itr != mem_end(data);) {
        const int res = write(file->handle, itr, mem_end(data) - itr);
        if (res > 0) {
            itr += res;
            continue;
        }

        switch (errno) {
            case EAGAIN:
            case EINTR: {
                continue;
            }
        }

        return fileresult_from_errno();
    }

    return FileResult_Success;
}

FileResult file_read_sync(File* file, DynString* dynstr) {
    diag_assert(file);

    Mem readBuffer = mem_stack(usize_kibibyte);
    while (true) {
        const ssize_t res = read(file->handle, readBuffer.ptr, readBuffer.size);
        if (res > 0) {
            dynstring_append(dynstr, mem_slice(readBuffer, 0, res));
            
            return FileResult_Success;
        }

        if (res == 0) {
            return FileResult_NoDataAvailable;
        }

        switch (errno) {
            case EINTR: {
                continue;
            }
        }

        return fileresult_from_errno();
    }
}

FileResult file_seek_sync(File* file, usize position) {
    if (lseek(file->handle, position, SEEK_SET) < 0) {
        return fileresult_from_errno();
    }

    return FileResult_Success;
}

FileResult file_delete_sync(String path) {
    if (path.size >= PATH_MAX) {
        return FileResult_PathTooLong;
    }

    Mem pathBuffer = mem_stack(PATH_MAX);
    mem_cpy(pathBuffer, path);
    *mem_at_u8(pathBuffer, path.size) = '\0';

    if (unlink((const char*)pathBuffer.ptr)) {
        return fileresult_from_errno();
    }

    return FileResult_Success;
}