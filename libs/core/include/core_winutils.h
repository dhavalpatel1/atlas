#pragma once

#include "core_string.h"

#ifdef ATLAS_WIN32

usize winutils_to_widestr_size(String input);

usize winutils_to_widestr(Mem output, String input);

usize winutils_from_widestr_size(void* input, usize inputCharCount);

usize winutils_from_widestr(String output, void* input, usize inputCharCount);

#endif
