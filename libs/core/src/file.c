/**
 * @file file.c
 * @brief Cross-platform file operations and result handling.
 *
 * This file provides the main interface for file operations in the Atlas core library.
 * It includes error result string mapping and initialization functions that delegate
 * to platform-specific implementations. The file system abstraction provides consistent
 * error handling across different operating systems.
 */

#include "core_array.h"
#include "core_diag.h"
#include "core_file.h"
#include "core_string.h"
#include "file_internal.h"
#include "init_internal.h"

/** @brief String representations of file operation result codes. */
static const String g_file_result_strs[] = {
    string_static("FileSuccess"),
    string_static("FileAlreadyExists"),
    string_static("FileDiskFull"),
    string_static("FileInvalidFilename"),
    string_static("FileLocked"),
    string_static("FileNoAccess"),
    string_static("FileNoDataAvailable"),
    string_static("FileNotFound"),
    string_static("FilePathTooLong"),
    string_static("FilePathInvalid"),
    string_static("FileTooManyOpen"),
    string_static("FileIsDirectory"),
    string_static("FileAllocationFailed"),
    string_static("FileUnkownError"),
};

_Static_assert(array_elems(g_file_result_strs) == FileResult_Count, "Incorrect number of FileResult strings");

/**
 * @brief Get a string representation of a file operation result code.
 *
 * This function returns a human-readable string description of the given
 * file operation result code, useful for debugging and error reporting.
 *
 * @param result The file result code to convert
 * @return A string describing the result code
 */
String file_result_str(FileResult result) {
    diag_assert(result < FileResult_Count);

    return g_file_result_strs[result];
}

/**
 * @brief Initialize the file system subsystem.
 *
 * This function initializes the platform-specific file system implementation.
 * It should be called once during application startup.
 */
void file_init() {
    file_pal_init();
}

FileResult file_read_to_end_sync(File* file, DynString* outData) {
    FileResult res;
    while ((res = file_read_sync(file, outData)) == FileResult_Success);

    return res == FileResult_NoDataAvailable ? FileResult_Success : res;
}