#pragma once 

#include "core_dynstring.h"

typedef u32 Utf8Codepoint;

bool utf8_contchar(u8 byte);

usize utf8_cp_count(String text);

usize utf8_cp_bytes(Utf8Codepoint cp);

void utf8_cp_write(DynString* str, Utf8Codepoint cp);