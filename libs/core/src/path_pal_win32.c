#include "core_diag.h"
#include "core_dynstring.h"
#include "core_memory.h"
#include "core_path.h"
#include "core_sentinel.h"
#include "core_string.h"
#include "core_winutils.h"
#include "path_internal.h"

#include <Windows.h>
#include <libloaderapi.h>
#include <processenv.h>

String path_pal_workingdir(Mem outputBuffer) {
    Mem utf16Temp = mem_stack(path_pal_max_size * sizeof(wchar_t) + 1);
    const usize utf16TempSize = GetCurrentDirectory(path_pal_max_size, utf16Temp.ptr);
    if (!utf16TempSize || utf16TempSize >= path_pal_max_size) {
        diag_crash_msg("GetCurrentDirectory() failed");
    }

    const usize utf8TempSize = winutils_from_widestr_size(utf16Temp.ptr, utf16TempSize);
    if (sentinel_check(utf8TempSize)) {
        diag_crash_msg("GetCurrentDirectory() malformed output");
    }

    Mem utf8Temp = mem_stack(utf8TempSize);
    winutils_from_widestr(utf8Temp, utf16Temp.ptr, utf16TempSize);

    DynString writer = dynstring_create_over(outputBuffer);
    path_canonize(&writer, utf8Temp);

    String result = dynstring_view(&writer);
    dynstring_destroy(&writer);

    return result;
}

String path_pal_executable(Mem outputBuffer) {
    Mem utf16Temp = mem_stack(path_pal_max_size * sizeof(wchar_t) + 1);
    const usize utf16TempSize = GetModuleFileName(null, utf16Temp.ptr, path_pal_max_size);
    if (!utf16TempSize || utf16TempSize >= path_pal_max_size) {
        diag_crash_msg("GetModuleFileName() failed");
    }

    const usize utf8TempSize = winutils_from_widestr_size(utf16Temp.ptr, utf16TempSize);
    if (sentinel_check(utf8TempSize)) {
        diag_crash_msg("GetModuleFileName() malformed output");
    }

    Mem utf8Temp = mem_stack(utf8TempSize);
    winutils_from_widestr(utf8Temp, utf16Temp.ptr, utf16TempSize);

    DynString writer = dynstring_create_over(outputBuffer);
    path_canonize(&writer, utf8Temp);

    String result = dynstring_view(&writer);
    dynstring_destroy(&writer);

    return result;
}