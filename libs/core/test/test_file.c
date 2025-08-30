#include "core_alloc.h"
#include "core_diag.h"
#include "core_file.h"
#include "core_path.h"
#include "core_rng.h"
#include "core_time.h"

#include "anvil_spec.h"

static void test_file_write_data(DynString* str, usize size) {
    for (usize i = 0; i != size; ++i) {
        dynstring_append_char(str, i % 255);
    }
}

static void test_file_verify_data(AnvilTestContext* _testCtx, String input) {
    for (usize i = 0; i != input.size; ++i) {
        anvil_eq_u64(*string_at(input, i), i % 255);
    }
}

spec(file) {

    File*     tmpFile   = null;
    DynString buffer = {0};

    setup() {
        file_temp(g_alloc_heap, &tmpFile);
        buffer = dynstring_create(g_alloc_page, usize_kibibyte * 4);
    }

    it("can read-back content that was written") {

        anvil_eq_u64(file_write_sync(tmpFile, string_lit("Hello World!")), FileResult_Success);
        anvil_eq_u64(file_seek_sync(tmpFile, 0), FileResult_Success);

        anvil_eq_u64(file_read_sync(tmpFile, &buffer), FileResult_Success);
        anvil_eq_string(dynstring_view(&buffer), string_lit("Hello World!"));
    }

    it("can read a file to the end") {
        const usize testDataSize = 2345;

        test_file_write_data(&buffer, testDataSize);
        anvil_eq_u64(file_write_sync(tmpFile, dynstring_view(&buffer)), FileResult_Success);
        anvil_eq_u64(file_seek_sync(tmpFile, 0), FileResult_Success);

        dynstring_clear(&buffer);
        anvil_eq_u64(file_read_to_end_sync(tmpFile, &buffer), FileResult_Success);

        anvil_eq_u64(buffer.size, testDataSize);
        test_file_verify_data(_testCtx, dynstring_view(&buffer));
    }

    it("can retrieve the file size") {
        anvil_eq_u64(file_stat_sync(tmpFile).size, 0);

        file_write_sync(tmpFile, string_lit("Hello World!"));
        anvil_eq_u64(file_stat_sync(tmpFile).size, 12);
    }

    it("can check the file-type of regular files") {
        anvil_eq_u64(file_stat_sync(tmpFile).type, FileType_Regular);
    }

    it("can check the file-type of directories") {
        File* workingDir = null;
        anvil_eq_u64(
            file_create(g_alloc_heap, g_path_workingdir, FileMode_Open, FileAccess_None, &workingDir),
            FileResult_Success);

        if (workingDir) {
            anvil_eq_u64(file_stat_sync(workingDir).type, FileType_Directory);
            file_destroy(workingDir);
        }
    }

    it("can retrieve the last access and last modification times") {
        const FileInfo info = file_stat_sync(tmpFile);
        anvil(time_real_duration(info.accessTime, time_real_clock()) < time_minute);
        anvil(time_real_duration(info.modTime, time_real_clock()) < time_minute);
    }

    it("can read file contents through a memory map") {
        file_write_sync(tmpFile, string_lit("Hello World!"));

        String mapping;
        anvil_eq_u64(file_map(tmpFile, &mapping), FileResult_Success);
        anvil_eq_string(mapping, string_lit("Hello World!"));
    }

    it("can write file contents through a memory map") {
        file_write_sync(tmpFile, string_lit("            "));

        String mapping;
        anvil_eq_u64(file_map(tmpFile, &mapping), FileResult_Success);
        mem_cpy(mapping, string_lit("Hello World!"));

        anvil_eq_string(mapping, string_lit("Hello World!"));
    }

    it("can check if a file exists") {
        File* nonExistingFile = null;
        anvil_eq_u64(
            file_create(
                g_alloc_heap,
                string_lit("path_to_non_existent_file_42"),
                FileMode_Open,
                FileAccess_Read,
                &nonExistingFile),
            FileResult_NotFound);
        anvil(nonExistingFile == null);
    }

    it("can read its own executable") {
        File* ownExecutable = null;
        anvil_eq_u64(
            file_create(
                g_alloc_heap, g_path_executable, FileMode_Open, FileAccess_Read, &ownExecutable),
            FileResult_Success);
        anvil(ownExecutable != null);

        anvil_eq_u64(file_read_sync(ownExecutable, &buffer), FileResult_Success);
        anvil(buffer.size > 0);

        if (ownExecutable) {
            file_destroy(ownExecutable);
        }
    }

    it("can create a new file by opening a file-handle with 'Create' mode") {
        String path = path_build_scratch(g_path_tempdir, path_random_name_scratch(g_rng, string_lit("atlas")));

        File* file;
        anvil_eq_u64(file_create(g_alloc_heap, path, FileMode_Create, FileAccess_Write, &file), FileResult_Success);
        anvil_eq_u64(file_write_sync(file, string_lit("Hello World!")), FileResult_Success);
        file_destroy(file);

        anvil_eq_u64(file_create(g_alloc_heap, path, FileMode_Open, FileAccess_Read, &file), FileResult_Success);
        anvil_eq_u64(file_read_sync(file, &buffer), FileResult_Success);
        anvil_eq_string(dynstring_view(&buffer), string_lit("Hello World!"));
        file_destroy(file);

        file_delete_sync(path);
    }

    it("can create a new directory") {
        String path = path_build_scratch(g_path_tempdir, path_random_name_scratch(g_rng, string_lit("atlas")));

        anvil_eq_u64(file_create_dir_sync(path), FileResult_Success);

        File* dirHandle;
        anvil_eq_u64(file_create(g_alloc_scratch, path, FileMode_Open, FileAccess_None, &dirHandle), FileResult_Success);
        if (dirHandle) {
            file_destroy(dirHandle);
        }

        anvil_eq_u64(file_delete_dir_sync(path), FileResult_Success);
    }

    teardown() {
        file_destroy(tmpFile);
        dynstring_destroy(&buffer);
    }
}
