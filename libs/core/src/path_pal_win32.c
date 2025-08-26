#include "core_diag.h"
#include "core_dynstring.h"
#include "core_env.h"
#include "core_memory.h"
#include "core_path.h"
#include "core_sentinel.h"
#include "core_string.h"
#include "core_winutils.h"
#include "path_internal.h"

#include <Windows.h>
#include <libloaderapi.h>
#include <processenv.h>

static String path_canonize_to_output_buffer(Mem outputBuffer, String path) {
    DynString writer = dynstring_create_over(outputBuffer);
    path_canonize(&writer, path);

    String result = dynstring_view(&writer);
    dynstring_destroy(&writer);

    return result;
}

String path_pal_workingdir(Mem outputBuffer) {
    Mem wideTemp = mem_stack((path_pal_max_size + 1) * sizeof(wchar_t));
    const usize wideTempSize = GetCurrentDirectory(path_pal_max_size, wideTemp.ptr);
    if (!wideTempSize || wideTempSize >= path_pal_max_size) {
        diag_crash_msg("GetCurrentDirectory() failed");
    }

    const usize utf8TempSize = winutils_from_widestr_size(wideTemp.ptr, wideTempSize);
    if (sentinel_check(utf8TempSize)) {
        diag_crash_msg("GetCurrentDirectory() malformed output");
    }

    Mem utf8Temp = mem_stack(utf8TempSize);
    winutils_from_widestr(utf8Temp, wideTemp.ptr, wideTempSize);

    return path_canonize_to_output_buffer(outputBuffer, utf8Temp);
}

String path_pal_executable(Mem outputBuffer) {
    Mem wideTemp = mem_stack((path_pal_max_size + 1) * sizeof(wchar_t));
    const usize wideTempSize = GetModuleFileName(null, wideTemp.ptr, path_pal_max_size);
    if (!wideTempSize || wideTempSize >= path_pal_max_size) {
        diag_crash_msg("GetModuleFileName() failed");
    }

    const usize utf8TempSize = winutils_from_widestr_size(wideTemp.ptr, wideTempSize);
    if (sentinel_check(utf8TempSize)) {
        diag_crash_msg("GetModuleFileName() malformed output");
    }

    Mem utf8Temp = mem_stack(utf8TempSize);
    winutils_from_widestr(utf8Temp, wideTemp.ptr, wideTempSize);

    return path_canonize_to_output_buffer(outputBuffer, utf8Temp);
}

String path_pal_tempdir(Mem outputBuffer) {
    DynString tmpWriter = dynstring_create_over(mem_stack(path_pal_max_size));
    String result;

    if (env_var(string_lit("TMPDIR"), &tmpWriter)) {
        result = path_canonize_to_output_buffer(outputBuffer, dynstring_view(&tmpWriter));
        goto Ret;
    }

    if (env_var(string_lit("TEMP"), &tmpWriter)) {
        result = path_canonize_to_output_buffer(outputBuffer, dynstring_view(&tmpWriter));
        goto Ret;
    }

    if (env_var(string_lit("TMP"), &tmpWriter)) {
        result = path_canonize_to_output_buffer(outputBuffer, dynstring_view(&tmpWriter));
        goto Ret;
    }

    diag_crash_msg("System temp directory could not be found");

Ret:
    dynstring_destroy(&tmpWriter);

    return result;
}