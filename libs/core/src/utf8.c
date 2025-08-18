#include "core_dynstring.h"
#include "core_memory.h"
#include "core_utf8.h"

/** @brief Maximum valid Unicode code point (U+10FFFF) */
#define utf8_cp_max ((Utf8Codepoint)0x10FFFF)

/** @brief Maximum code point that fits in 1 UTF-8 byte (U+007F) */
#define utf8_cp_single_char ((Utf8Codepoint)0x7F)

/** @brief Maximum code point that fits in 2 UTF-8 bytes (U+07FF) */
#define utf8_cp_double_char ((Utf8Codepoint)0x7FF)

/** @brief Maximum code point that fits in 3 UTF-8 bytes (U+FFFF) */
#define utf8_cp_triple_char ((Utf8Codepoint)0xFFFF)

/** @brief Maximum code point that fits in 4 UTF-8 bytes (same as utf8_cp_max) */
#define utf8_cp_quad_char utf8_cp_max

/**
    * @brief Check if a Unicode code point is valid
    * @param cp Code point to validate
    * @return true if the code point is within the valid Unicode range, false otherwise
    */
static bool utf8_cp_valid(const Utf8Codepoint cp) {
    return cp <= utf8_cp_max;
}

bool utf8_contchar(u8 byte) {
    return (byte & 0b11000000) == 0b10000000;
}

usize utf8_cp_count(String str) {
    usize result = 0;
    mem_for_u8(str, byte, {
        if (!utf8_contchar(byte)) {
            ++result;
        }
    });

    return result;
}

usize utf8_cp_bytes(const Utf8Codepoint cp) {
    if (cp <= utf8_cp_single_char) {
        return 1;
    }

    if (cp <= utf8_cp_double_char) {
        return 2;
    }

    if (cp <= utf8_cp_triple_char) {
        return 3;
    }

    return 4;
}

void utf8_cp_write(DynString* str, const Utf8Codepoint cp) {
    if (!utf8_cp_valid(cp)) {
        // Write UTF-8 replacement character (U+FFFD): EF BF BD
        dynstring_append_char(str, 0xEF);
        dynstring_append_char(str, 0xBF);
        dynstring_append_char(str, 0xBD);

        return;
    }

    if (cp <= utf8_cp_single_char) {
        dynstring_append_char(str, (u8)cp);

        return;
    }

    if (cp <= utf8_cp_double_char) {
        // 2-byte sequence: 110xxxxx 10xxxxxx
        dynstring_append_char(str, (u8)(((cp >> 6) & 0x1F) | 0xC0));
        dynstring_append_char(str, (u8)((cp & 0x3F) | 0x80));
        
        return;
    }

    if (cp <= utf8_cp_triple_char) {
        // 3-byte sequence: 1110xxxx 10xxxxxx 10xxxxxx
        dynstring_append_char(str, (u8)(((cp >> 12) & 0x0F) | 0xE0));
        dynstring_append_char(str, (u8)(((cp >> 6) & 0x3F) | 0x80));
        dynstring_append_char(str, (u8)((cp & 0x3F) | 0x80));

        return;
    }

    // 4-byte sequence: 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
    dynstring_append_char(str, (u8)(((cp >> 18) & 0x07) | 0xF0));
    dynstring_append_char(str, (u8)(((cp >> 12) & 0x3F) | 0x80));
    dynstring_append_char(str, (u8)(((cp >> 6) & 0x3F) | 0x80));
    dynstring_append_char(str, (u8)((cp & 0x3F) | 0x80));
}