#pragma once

#include "core_alloc.h"
#include "core_dynstring.h"
#include "core_string.h"

typedef i64 TimeReal;

typedef struct sFile File;

typedef enum {
    FileResult_Success = 0,
    FileResult_AlreadyExists,
    FileResult_DiskFull,
    FileResult_InvalidFilename,
    FileResult_Locked,
    FileResult_NoAccess,
    FileResult_NoDataAvailable,
    FileResult_NotFound,
    FileResult_PathTooLong,
    FileResult_PathInvalid,
    FileResult_TooManyOpenFiles,
    FileResult_IsDirectory,
    FileResult_AllocationFailed,
    FileResult_UnknownError,

    FileResult_Count,
} FileResult;

typedef enum {
    FileMode_Open,
    FileMode_Append,
    FileMode_Create,
} FileMode;

typedef enum {
    FileAccess_None = 0,
    FileAccess_Read = 1 << 0,
    FileAccess_Write = 1 << 1,
} FileAccessFlags;

typedef enum {
    FileType_Regular,
    FileType_Directory,
    FileType_Unknown,
} FileType;

typedef struct {
    usize size;
    FileType type;
    TimeReal accessTime;
    TimeReal modTime;
} FileInfo;

extern File* g_file_stdin;
extern File* g_file_stdout;
extern File* g_file_stderr;

String file_result_str(FileResult result);

FileResult file_create(Allocator* allocator, String path, FileMode mode, FileAccessFlags flags, File** file);

FileResult file_temp(Allocator* allocator, File** file);

void file_destroy(File* file);

FileResult file_write_sync(File* file, String data);

FileResult file_read_sync(File* file, DynString* outData);

FileResult file_read_to_end_sync(File* file, DynString* outData);

FileResult file_seek_sync(File* file, usize position);

FileInfo file_stat_sync(File* file);

FileResult file_delete_sync(String path);

FileResult file_map(File* file, String* output);