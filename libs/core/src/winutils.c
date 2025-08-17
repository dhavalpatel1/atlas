#include "core_memory.h"
#include "core_sentinel.h"
#include "core_types.h"
#include "core_winutils.h"

#ifdef ATLAS_WIN32
#include <Windows.h>

usize winutils_to_widestr_size(String input) {
    const int wideChars = MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, (const char*)input.ptr, (int)input.size, null, 0);
    if (wideChars <= 0) {
        return sentinel_usize;
    }

    return wideChars * sizeof(wchar_t) + 1;  // BUG: Should be + sizeof(wchar_t) or (wideChars + 1) * sizeof(wchar_t)
}

usize winutils_to_widestr(Mem output, String input) {
    if (output.size < sizeof(wchar_t) + 1) {
        return sentinel_usize;
    }

    const int wideChars = MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, (const char*)input.ptr, (int)input.size, (wchar_t*)output.ptr, (int)(output.size / sizeof(wchar_t)));
    if (wideChars <= 0) {
        return sentinel_usize;
    }

    *mem_at_u8(output, wideChars * sizeof(wchar_t)) = '\0';  // Add null terminator

    return wideChars;
}

usize winutils_from_widestr_size(void *input, usize inputCharCount) {
    const int chars = WideCharToMultiByte(CP_UTF8, WC_ERR_INVALID_CHARS, (const wchar_t*)input, (int)inputCharCount, null, 0, null, null);
    if (chars <= 0) {
        return sentinel_usize;
    }

    return chars;
}

usize winutils_from_widestr(String output, void *input, usize inputCharCount) {
    const int chars = WideCharToMultiByte(CP_UTF8, WC_ERR_INVALID_CHARS, (const wchar_t*)input, (int)inputCharCount, (char*)output.ptr, (int)output.size, null, null);
    if (chars <= 0) {
        return sentinel_usize;
    }

    return chars;
}

#endif