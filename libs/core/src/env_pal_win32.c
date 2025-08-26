#include "core_alloc.h"
#include "core_diag.h"
#include "core_env.h"
#include "core_winutils.h"

#include <Windows.h>

#define env_var_max_name_size 256
#define env_var_max_value_size (usize_kibibyte * 32)

bool env_var(String name, DynString* output) {
    const usize nameBufferSize = winutils_to_widestr_size(name);
    if (UNLIKELY(sentinel_check(nameBufferSize))) {
        return false;
    }

    if (UNLIKELY(nameBufferSize >= env_var_max_name_size)) {
        diag_assert_fail("Environment variable name with length {} exceeds maximum of {}",
        fmt_int(nameBufferSize),
        fmt_int(env_var_max_name_size));

        return false;
    }

    Mem nameBufferMem = mem_stack(nameBufferSize);
    winutils_to_widestr(nameBufferMem, name);

    Mem buffer = alloc_alloc(g_alloc_scratch, env_var_max_value_size, 1);
    const DWORD bufferMaxChars = (DWORD)(buffer.size / sizeof(wchar_t));

    const DWORD wcharCount = GetEnvironmentVariable((const wchar_t*)nameBufferMem.ptr, (wchar_t*)buffer.ptr, bufferMaxChars);

    if (wcharCount == 0) {
        alloc_free(g_alloc_scratch, buffer);

        return false;
    }

    if (UNLIKELY(wcharCount >= bufferMaxChars)) {
        alloc_free(g_alloc_scratch, buffer);

        return false;
    }

    if (output) {
        const usize outputSize = winutils_from_widestr_size(buffer.ptr, (usize)wcharCount);
        if (sentinel_check(outputSize)) {
            diag_crash_msg("GetEnvironmentVariable() malformed output");
        }

        winutils_from_widestr(dynstring_push(output, outputSize), buffer.ptr, (usize)wcharCount);
    }

    alloc_free(g_alloc_scratch, buffer);
    return true;
}