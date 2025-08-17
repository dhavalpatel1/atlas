#include "core_alloc.h"
#include "core_array.h"
#include "core_base64.h"

static u8 g_decodeTable[] = {
    62,  255, 255, 255, 63,  52,  53, 54, 55, 56, 57, 58, 59, 60, 61, 255,
    255, 255, 255, 255, 255, 255, 0,  1,  2,  3,  4,  5,  6,  7,  8,  9,
    10,  11,  12,  13,  14,  15,  16, 17, 18, 19, 20, 21, 22, 23, 24, 25,
    255, 255, 255, 255, 255, 255, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35,
    36,  37,  38,  39,  40,  41,  42, 43, 44, 45, 46, 47, 48, 49, 50, 51
};

_Static_assert(sizeof(g_decodeTable) == 'z' - '+' + 1, "Incorrect decode table size");

usize base64_decoded_size(String encoded) {
    if (encoded.size < 2) {
        return 0;
    }

    const u8 padding = *(string_end(encoded) - 2) == '=' ? 2U : *(string_end(encoded) - 1) == '=' ? 1U : 0u;

    return encoded.size / 4 * 3 - padding;
}

void base64_decode(DynString* str, String encoded) {
    u32 val = 0;
    i32 valBits = -8;
    mem_for_u8(encoded, c, {
        if (c < '+' || c > 'z') {
            break;
        }

        if (g_decodeTable[c - '+'] == 255) {
            break;
        }

        val = (val << 6U) | g_decodeTable[c - '+'];
        valBits += 6;
        if (valBits >= 0) {
            dynstring_append_char(str, (u8)(val >> valBits));
            valBits -= 8;
        }
    });
}

String base64_decode_scratch(String encoded) {
    const usize decodedSize = base64_decoded_size(encoded);
    if (!decodedSize) {
        return string_empty;
    }

    Mem scratchMem = alloc_alloc(g_alloc_scratch, decodedSize, 1);
    DynString str = dynstring_create_over(scratchMem);

    base64_decode(&str, encoded);

    String res = dynstring_view(&str);
    dynstring_destroy(&str);

    return res;
}