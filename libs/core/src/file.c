#include "core_alloc.h"
#include "core_array.h"
#include "core_diag.h"
#include "core_file.h"
#include "core_path.h"
#include "core_string.h"

#include "file_internal.h"
#include "init_internal.h"

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

String file_result_str(FileResult result) {
    diag_assert(result < FileResult_Count);

    return g_file_result_strs[result];
}

void file_init() {
    file_pal_init();
}

FileResult file_read_to_end_sync(File* file, DynString* outData) {
    FileResult res;
    while ((res = file_read_sync(file, outData)) == FileResult_Success);

    return res == FileResult_NoDataAvailable ? FileResult_Success : res;
}

FileResult file_create_dir_sync(String path) {
    File* dirHandle;
    FileResult res;

    res = file_create(g_alloc_scratch, path, FileMode_Open, FileAccess_None, &dirHandle);
    if (res == FileResult_Success) {
        file_destroy(dirHandle);

        return FileResult_Success;
    }

    res = file_create_dir_sync(path_parent(path));
    if (res != FileResult_Success) {
        return res;
    }

    return file_pal_create_dir_single_sync(path);
}
