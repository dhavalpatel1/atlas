/**
 * @file core_file.h
 * @brief File I/O operations and file management
 *
 * This header provides cross-platform file operations including reading, writing,
 * seeking, and file management. It supports both synchronous I/O operations and
 * standard stream handles.
 */

#pragma once

#include "core_alloc.h"
#include "core_dynstring.h"
#include "core_string.h"

/** @brief Opaque file handle structure */
typedef struct sFile File;

/**
 * @brief File operation result codes
 */
typedef enum {
    FileResult_Success = 0,           /**< Operation completed successfully */
    FileResult_AlreadyExists,         /**< File already exists */
    FileResult_DiskFull,              /**< Insufficient disk space */
    FileResult_InvalidFilename,       /**< Invalid filename or path */
    FileResult_Locked,                /**< File is locked by another process */
    FileResult_NoAccess,              /**< Access denied */
    FileResult_NoDataAvailable,       /**< No data available for reading */
    FileResult_NotFound,              /**< File or path not found */
    FileResult_PathTooLong,           /**< Path exceeds maximum length */
    FileResult_PathInvalid,           /**< Path contains invalid characters */
    FileResult_TooManyOpenFiles,      /**< Too many files open */
    FileResult_IsDirectory,           /**< Operation attempted on a directory */
    FileResult_UnknownError,          /**< Unknown or unspecified error */

    FileResult_Count,                 /**< Number of result codes */
} FileResult;

/**
 * @brief File opening modes
 */
typedef enum {
    FileMode_Open,                    /**< Open existing file */
    FileMode_Append,                  /**< Open for appending, create if needed */
    FileMode_Create,                  /**< Create new file, truncate if exists */
} FileMode;

/**
 * @brief File access permission flags
 */
typedef enum {
    FileAccess_Read  = 1 << 0,        /**< Read access permission */
    FileAccess_Write = 1 << 1,        /**< Write access permission */
} FileAccessFlags;

/** @brief Global standard input file handle */
extern File* g_file_stdin;
/** @brief Global standard output file handle */
extern File* g_file_stdout;
/** @brief Global standard error file handle */
extern File* g_file_stderr;

/**
 * @brief Get a string representation of a file result code
 * @param result File result code to convert
 * @return String describing the result code
 */
String file_result_str(FileResult result);

/**
 * @brief Create or open a file with specified mode and access flags
 * @param allocator Allocator for file handle allocation
 * @param path Path to the file
 * @param mode File opening mode
 * @param flags Access permission flags
 * @param file Output pointer for the created file handle
 * @return FileResult indicating success or failure
 */
FileResult file_create(Allocator* allocator, String path, FileMode mode, FileAccessFlags flags, File** file);

/**
 * @brief Create a temporary file
 * @param allocator Allocator for file handle allocation
 * @param file Output pointer for the created temporary file handle
 * @return FileResult indicating success or failure
 */
FileResult file_temp(Allocator* allocator, File** file);

/**
 * @brief Close and destroy a file handle
 * @param file File handle to destroy
 */
void file_destroy(File* file);

/**
 * @brief Write data to a file synchronously
 * @param file File handle to write to
 * @param data Data to write
 * @return FileResult indicating success or failure
 */
FileResult file_write_sync(File* file, String data);

/**
 * @brief Read all data from a file synchronously
 * @param file File handle to read from
 * @param outData Dynamic string to store the read data
 * @return FileResult indicating success or failure
 */
FileResult file_read_sync(File* file, DynString* outData);

/**
 * @brief Seek to a specific position in a file
 * @param file File handle to seek in
 * @param position Byte offset from the beginning of the file
 * @return FileResult indicating success or failure
 */
FileResult file_seek_sync(File* file, usize position);

/**
 * @brief Delete a file from the filesystem
 * @param path Path to the file to delete
 * @return FileResult indicating success or failure
 */
FileResult file_delete_sync(String path);
