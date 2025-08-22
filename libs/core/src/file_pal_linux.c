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

#include "linux/limits.h"

#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

#include "file_internal.h"
#include "time_internal.h"

/**
 * @brief Internal structure for managing memory-mapped files on Linux
 * 
 * Stores the memory mapping information needed for proper cleanup
 * when the file is destroyed or the mapping is released.
 */
typedef struct {
    void* addr;     /**< Memory address of the mapped region */
    usize size;     /**< Size of the mapped region in bytes */
} FileMapping;

/** @brief Standard input file handle */
File* g_file_stdin = &(File){ .handle = 0, .access = FileAccess_Read };

/** @brief Standard output file handle */
File* g_file_stdout = &(File){ .handle = 1, .access = FileAccess_Write };

/** @brief Standard error file handle */
File* g_file_stderr = &(File){ .handle = 2, .access = FileAccess_Write };

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

    if ((access & FileAccess_Read) && (access & FileAccess_Write)) {
        flags |= O_RDWR;
    } else if (access & FileAccess_Read) {
        flags |= O_RDONLY;
    } else if (access & FileAccess_Write) {
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
        .access = access,
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
        .access = FileAccess_Read | FileAccess_Write,
        .allocator = allocator
    };

    return FileResult_Success;
}

void file_destroy(File* file) {
    diag_assert_msg(file->allocator, "Invalid file");

    if (file->mapping) {
        FileMapping* mapping = file->mapping;
        const int res = munmap(mapping->addr, mapping->size);
        if (UNLIKELY(res != 0)) {
            diag_crash_msg("munmap() failed: {}", fmt_int(res));
        }

        alloc_free_t(file->allocator, mapping);
    }

    close(file->handle);
    alloc_free_t(file->allocator, file);
}

FileResult file_write_sync(File* file, const String data) {
    diag_assert(file);
    diag_assert_msg(file->access & FileAccess_Write, "File handle does not have write access");

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
    diag_assert_msg(file->access & FileAccess_Read, "File handle does not have read access");

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

/**
 * @brief Get file metadata and statistics using Linux fstat()
 * @param file File handle to query
 * @return FileInfo structure with file metadata
 * 
 * Uses the Linux fstat() system call to retrieve file metadata including
 * size and timestamps. Converts Linux-specific timespec structures to
 * cross-platform TimeReal format.
 */
FileInfo file_stat_sync(File* file) {
    struct stat statOutput;
    const int res = fstat(file->handle, &statOutput);
    if (UNLIKELY(res != 0)) {
        diag_crash_msg("fstat() failed: {}", fmt_int(res));
    }

    return (FileInfo) {
        .size = statOutput.st_size,
        .accessTime = time_pal_native_to_real(statOutput.st_atim),
        .modTime = time_pal_native_to_real(statOutput.st_mtim),
    };
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

/**
 * @brief Memory-map a file using Linux mmap() system call
 * @param file File handle to map
 * @param output Pointer to receive the mapped memory region as a String view
 * @return FileResult indicating success or failure
 * 
 * Creates a memory mapping of the entire file using the Linux mmap() system call.
 * The protection flags are determined by the file's access permissions:
 * - Read access enables PROT_READ
 * - Write access enables PROT_WRITE
 * 
 * Uses MAP_SHARED so that changes are visible to other processes and are
 * written back to the file. The mapping is automatically cleaned up when
 * the file is destroyed.
 */
FileResult file_map(File* file, String* output) {
    diag_assert_msg(!file->mapping, "File is already mapped");

    const usize size = file_stat_sync(file).size;

    // Determine memory protection flags based on file access permissions
    int prot = 0;
    if (file->access & FileAccess_Read) {
        prot |= PROT_READ;
    }

    if (file->access & FileAccess_Write) {
        prot |= PROT_WRITE;
    }

    // Create the memory mapping
    void* addr = mmap(null, size, prot, MAP_SHARED, file->handle, 0);
    if (UNLIKELY(!addr)) {
        return fileresult_from_errno();
    }

    // Allocate tracking structure for cleanup
    file->mapping = alloc_alloc_t(file->allocator, FileMapping);
    if (UNLIKELY(!file->mapping)) {
        const int res = munmap(addr, size);
        if (UNLIKELY(res != 0)) {
            diag_crash_msg("munmap() failed: {}", fmt_int(res));
        }

        return FileResult_AllocationFailed;
    }

    *(FileMapping*)file->mapping = (FileMapping){
        .addr = addr,
        .size = size,
    };

    *output = mem_create(addr, size);

    return FileResult_Success;
}