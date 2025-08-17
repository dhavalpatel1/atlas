/**
 * @file file_pal_win32.c
 * @brief Windows platform abstraction layer for file operations
 *
 * This file implements the Windows-specific file system operations including
 * file creation, reading, writing, deletion, and directory management.
 * It translates between the cross-platform file API and Windows API calls.
 */

#include "core_alloc.h"
#include "core_diag.h"
#include "core_dynstring.h"
#include "core_file.h"
#include "core_memory.h"
#include "core_sentinel.h"
#include "core_string.h"
#include "core_types.h"
#include "core_winutils.h"
#include "file_internal.h"
#include "path_internal.h"

#include <Windows.h>
#include <errhandlingapi.h>
#include <fileapi.h>
#include <handleapi.h>
#include <minwindef.h>
#include <processenv.h>
#include <wchar.h>
#include <winbase.h>
#include <winerror.h>
#include <winnt.h>

/** @brief Standard input file handle (initialized at runtime) */
File* g_file_stdin;
/** @brief Standard output file handle (initialized at runtime) */
File* g_file_stdout;
/** @brief Standard error file handle (initialized at runtime) */
File* g_file_stderr;

void file_pal_init() {
    static File stdIn = {0};
    stdIn.handle = GetStdHandle(STD_INPUT_HANDLE);
    if (stdIn.handle != INVALID_HANDLE_VALUE) {
        g_file_stdin = &stdIn;
    }

    static File stdOut = {0};
    stdOut.handle = GetStdHandle(STD_OUTPUT_HANDLE);
    if (stdOut.handle != INVALID_HANDLE_VALUE) {
        g_file_stdout = &stdOut;
    }

    static File stdErr = {0};
    stdErr.handle = GetStdHandle(STD_ERROR_HANDLE);
    if (stdErr.handle != INVALID_HANDLE_VALUE) {
        g_file_stderr = &stdErr;
    }
}

/**
 * @brief Convert Windows error codes to cross-platform FileResult codes
 *
 * This function maps Windows-specific error codes from GetLastError() to the
 * platform-independent FileResult enumeration for consistent error
 * handling across different operating systems.
 *
 * @return FileResult code corresponding to the current Windows error
 */
static FileResult fileresult_from_lasterror() {
    switch (GetLastError()) {
        case ERROR_ACCESS_DENIED: {
            return FileResult_NoAccess;
        }

        case ERROR_SHARING_VIOLATION: {
            return FileResult_Locked;
        }

        case ERROR_FILE_NOT_FOUND:
        case ERROR_PATH_NOT_FOUND:
        case ERROR_INVALID_DRIVE: {
            return FileResult_NotFound;
        }

        case ERROR_DISK_FULL: {
            return FileResult_DiskFull;
        }

        case ERROR_TOO_MANY_OPEN_FILES: {
            return FileResult_TooManyOpenFiles;
        }

        case ERROR_BUFFER_OVERFLOW: {
            return FileResult_PathTooLong;
        }

        case ERROR_INVALID_NAME:
        case ERROR_BAD_PATHNAME: {
            return FileResult_InvalidFilename;
        }

        case ERROR_FILE_EXISTS:
        case ERROR_ALREADY_EXISTS: {
            return FileResult_AlreadyExists;
        }
    }

    return FileResult_UnknownError;
}

FileResult file_create(Allocator* alloc, String path, FileMode mode, FileAccessFlags access, File** file) {
    const usize pathBufferSize = winutils_to_widestr_size(path);
    if (sentinel_check(pathBufferSize)) {
        return FileResult_PathInvalid;
    }

    if (pathBufferSize > path_pal_max_size) {
        return FileResult_PathTooLong;
    }

    Mem pathBufferMem = mem_stack(pathBufferSize);
    winutils_to_widestr(pathBufferMem, path);

    DWORD shareMode = FILE_SHARE_READ | FILE_SHARE_WRITE;
    DWORD desiredAccess = 0;
    DWORD createDisposition = 0;
    DWORD flags = FILE_ATTRIBUTE_NORMAL | FILE_FLAG_POSIX_SEMANTICS;

    switch (mode) {
        case FileMode_Open: {
            createDisposition = OPEN_EXISTING;
        } break;

        case FileMode_Append: {
            desiredAccess |= FILE_APPEND_DATA;
            createDisposition = OPEN_ALWAYS;
        } break;

        case FileMode_Create: {
            createDisposition = CREATE_ALWAYS;
        } break;

        default: {
            diag_assert_fail("Invalid FileMode: {}", fmt_int(mode));
        }
    }

    if (access & FileAccess_Read) {
        desiredAccess |= GENERIC_READ;
    }
    
    if (mode != FileMode_Append && access & FileAccess_Write) {
        desiredAccess |= GENERIC_WRITE;
    }

    HANDLE handle = CreateFile((const wchar_t*)pathBufferMem.ptr, desiredAccess, shareMode, null, createDisposition, flags, null);
    if (handle == INVALID_HANDLE_VALUE) {
        return fileresult_from_lasterror();
    }

    *file = alloc_alloc_t(alloc, File);
    **file = (File) {
        .handle = handle,
        .allocator = alloc,
    };

    return FileResult_Success;
}

FileResult file_temp(Allocator *allocator, File **file) {
    Mem tempDirPath = mem_stack(MAX_PATH * sizeof(wchar_t) + 1);
    const DWORD tempDirChars = GetTempPath(MAX_PATH, (wchar_t*)tempDirPath.ptr);
    if (!tempDirChars) {
        return fileresult_from_lasterror();
    }

    Mem tempFilePath = mem_stack(MAX_PATH * sizeof(wchar_t));
    if (GetTempFileName((const wchar_t*)tempDirPath.ptr, TEXT("vol"), 0, (wchar_t*)tempFilePath.ptr) == 0) {
        return fileresult_from_lasterror();
    }

    HANDLE handle = CreateFile((const wchar_t*)tempFilePath.ptr, GENERIC_READ | GENERIC_WRITE, 0, null, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_DELETE_ON_CLOSE, null);
    if (handle == INVALID_HANDLE_VALUE) {
        return fileresult_from_lasterror();
    }

    *file = alloc_alloc_t(allocator, File);
    **file = (File) {
        .handle = handle,
        .allocator = allocator
    };

    return FileResult_Success;
}

void file_destroy(File *file) {
    diag_assert_msg(file->allocator, "Invalid file");
    CloseHandle(file->handle);
    alloc_free_t(file->allocator, file);
}

FileResult file_write_sync(File* file, const String data) {
    diag_assert(file);

    for (u8* itr = mem_begin(data); itr != mem_end(data);) {
        DWORD bytesWritten;
        if (WriteFile(file->handle, itr, (DWORD)(mem_end(data) - itr), &bytesWritten, null)) {
            itr += bytesWritten;

            continue;
        }

        return fileresult_from_lasterror();
    }

    return FileResult_Success;
}

FileResult file_read_sync(File *file, DynString *dynStr) {
    diag_assert(file);

    Mem readBuffer = mem_stack(usize_kibibyte);
    DWORD bytesRead;
    BOOL success = ReadFile(file->handle, readBuffer.ptr, (DWORD)readBuffer.size, &bytesRead, null);
    if (success && bytesRead) {
        dynstring_append(dynStr, mem_slice(readBuffer, 0, bytesRead));

        return FileResult_Success;
    }

    if (success) {
        return FileResult_NoDataAvailable;
    }

    return fileresult_from_lasterror();
}

FileResult file_seek_sync(File *file, usize position) {
    LARGE_INTEGER li;
    li.QuadPart = position;
    li.LowPart = SetFilePointer(file->handle, li.LowPart, &li.HighPart, FILE_BEGIN);

    if (li.LowPart == INVALID_SET_FILE_POINTER && GetLastError() != NO_ERROR) {
        return fileresult_from_lasterror();
    }

    return FileResult_Success;
}

FileResult file_delete_sync(String path) {
    const usize pathBufferSize = winutils_to_widestr_size(path);
    if (sentinel_check(pathBufferSize)) {
        return FileResult_PathInvalid;
    }

    if (pathBufferSize > path_pal_max_size) {
        return FileResult_PathTooLong;
    }

    Mem pathBufferMem = mem_stack(pathBufferSize);
    winutils_to_widestr(pathBufferMem, path);

    const BOOL success = DeleteFile(pathBufferMem.ptr);

    return success ? FileResult_Success : fileresult_from_lasterror();
}