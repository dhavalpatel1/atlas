#include "core_alloc.h"
#include "core_annotation.h"
#include "core_array.h"
#include "core_ascii.h"
#include "core_bitset.h"
#include "core_diag.h"
#include "core_dynstring.h"
#include "core_float.h"
#include "core_format.h"
#include "core_math.h"
#include "core_memory.h"
#include "core_path.h"
#include "core_sentinel.h"
#include "core_string.h"
#include "core_time.h"
#include "core_tty.h"
#include "core_types.h"
#include <stddef.h>

typedef enum {
    FormatReplOptKind_None = 0,
    FormatReplOptKind_PadLeft,
    FormatReplOptKind_PadRight,
    FormatReplOptKind_PadCenter,
} FormatReplOptKind;

typedef struct {
    FormatReplOptKind kind;
    i32 value;
} FormatReplOpt;

typedef struct {
    usize start;
    usize end;
    FormatReplOpt opt;
} FormatRepl;

static FormatReplOpt format_replacement_parse_opt(String str) {
    str = format_read_whitespace(str, null);
    FormatReplOpt result = (FormatReplOpt) {
        .kind = FormatReplOptKind_None
    };

    if (str.size) {
        switch (*string_begin(str)) {
            case '>': {
                result.kind = FormatReplOptKind_PadLeft;
                str = string_consume(str, 1);
            } break;

            case '<': {
                result.kind = FormatReplOptKind_PadRight;
                str = string_consume(str, 1);
            } break;

            case ':': {
                result.kind = FormatReplOptKind_PadCenter;
                str = string_consume(str, 1);
            } break;
        }

        if (result.kind) {
            u64 amount;
            str = format_read_u64(str, &amount, 10);
            result.value = (i32)amount;
        }
    }

    str = format_read_whitespace(str, null);

    diag_assert_msg(!str.size, "Unsupported format option: '{}'", fmt_text(str, .flags = FormatTextFlags_EscapeNonPrintAscii));

    return result;
}

static bool format_replacement_find(String str, FormatRepl* result) {
    const usize startIdx = string_find_first(str, string_lit("{"));
    if (sentinel_check(startIdx)) {
        return false;
    }

    const usize len = string_find_first(string_consume(str, startIdx), string_lit("}"));
    if (sentinel_check(len)) {
        return false;
    }

    *result = (FormatRepl) {
        .start = startIdx,
        .end = startIdx + len + 1,
        .opt = format_replacement_parse_opt(string_slice(str, startIdx + 1, len - 1)),
    };

    return true;
}

void format_write_formatted(DynString *dynstr, String format, const FormatArg* argHead) {
    while (format.size) {
        FormatRepl repl;
        if (!format_replacement_find(format, &repl)) {
            dynstring_append(dynstr, format);

            break;
        }

        dynstring_append(dynstr, string_slice(format, 0, repl.start));
        if (argHead->type != FormatArgType_End) {
            const usize argStart = dynstr->size;
            format_write_arg(dynstr, argHead);
            const usize argEnd = dynstr->size;

            switch (repl.opt.kind) {
                case FormatReplOptKind_None: {

                } break;

                case FormatReplOptKind_PadLeft: {
                    const usize padding = math_max(0, repl.opt.value - (i32)(argEnd - argStart));
                    dynstring_insert_chars(dynstr, ' ', argStart, padding);
                } break;

                case FormatReplOptKind_PadRight: {
                    const usize padding = math_max(0, repl.opt.value - (i32)(argEnd - argStart));
                    dynstring_append_chars(dynstr, ' ', padding);
                } break;

                case FormatReplOptKind_PadCenter: {
                    const usize padding = math_max(0, repl.opt.value - (i32)(argEnd - argStart));
                    dynstring_insert_chars(dynstr, ' ', argStart, padding / 2);
                    dynstring_append_chars(dynstr, ' ', padding / 2 + padding % 2);
                } break;
            }

            ++argHead;
        }

        format = string_consume(format, repl.end);
    }
}

String format_write_formatted_scratch(String format, const FormatArg *args) {
    Mem scratchMem = alloc_alloc(g_alloc_scratch, usize_kibibyte * 8, 1);
    DynString str = dynstring_create_over(scratchMem);

    format_write_formatted(&str, format, args);

    String res = dynstring_view(&str);
    dynstring_destroy(&str);

    return res;
}

void format_write_arg(DynString *dynstr, const FormatArg *arg) {
    switch (arg->type) {
        case FormatArgType_End:
        case FormatArgType_Nop: {
        } break;

        case FormatArgType_List: {
            dynstring_append(dynstr, ((const FormatOptsList*)arg->settings)->prefix);
            for (const FormatArg* child = arg->value_list; child->type != FormatArgType_End; ++child) {
                if (child != arg->value_list) {
                    dynstring_append(dynstr, ((const FormatOptsList*)arg->settings)->seperator);
                }

                format_write_arg(dynstr, child);
            }

            dynstring_append(dynstr, ((const FormatOptsList*)arg->settings)->suffix);
        } break;

        case FormatArgType_i64: {
            format_write_i64(dynstr, arg->value_i64, arg->settings);
        } break;

        case FormatArgType_u64: {
            format_write_u64(dynstr, arg->value_u64, arg->settings);
        } break;

        case FormatArgType_f64: {
            format_write_f64(dynstr, arg->value_f64, arg->settings);
        } break;

        case FormatArgType_bool: {
            format_write_bool(dynstr, arg->value_bool);
        } break;

        case FormatArgType_BitSet: {
            format_write_bitset(dynstr, arg->value_bitset);
        } break;

        case FormatArgType_Mem: {
            format_write_mem(dynstr, arg->value_mem);
        } break;

        case FormatArgType_Duration: {
            format_write_time_duration_pretty(dynstr, arg->value_duration);
        } break;

        case FormatArgType_Time: {
            format_write_time_iso8601(dynstr, arg->value_time, arg->settings);
        } break;

        case FormatArgType_Size: {
            format_write_size_pretty(dynstr, arg->value_size);
        } break;

        case FormatArgType_Text: {
            format_write_text(dynstr, arg->value_text, arg->settings);
        } break;

        case FormatArgType_Char: {
            format_write_char(dynstr, arg->value_char, arg->settings);
        }

        case FormatArgType_Path: {
            path_canonize(dynstr, arg->value_path);
        } break;

        case FormatArgType_TtyStyle: {
            tty_write_style_sequence(dynstr, arg->value_ttystyle);
        } break;

        case FormatArgType_Padding: {
            dynstring_append_chars(dynstr, ' ', arg->value_padding);
        } break;
    }
}

String format_write_arg_scratch(const FormatArg* arg) {
    Mem scratchMem = alloc_alloc(g_alloc_scratch, usize_kibibyte, 1);
    DynString str = dynstring_create_over(scratchMem);

    format_write_arg(&str, arg);

    String res = dynstring_view(&str);
    dynstring_destroy(&str);

    return res;
}

void format_write_u64(DynString *dynstr, u64 val, const FormatOptsInt *opts) {
    diag_assert(opts->base > 1 && opts->base <= 16);

    Mem buffer = mem_stack(64);
    u8* ptr = mem_end(buffer);

    const char* chars = "0123456789ABCDEF";
    u8 digitsWritten = 0;

    do {
        *--ptr = chars[val % opts->base];
        val /= opts->base;
    } while (++digitsWritten < opts->minDigits || val);

    const u8 numDigits = (u8)(mem_end(buffer) - ptr);
    dynstring_append(dynstr, mem_create(ptr, numDigits));
}

void format_write_i64(DynString *dynstr, i64 val, const FormatOptsInt *opts) {
    if (val < 0) {
        dynstring_append_char(dynstr, '-');
        val = -val;
    }

    format_write_u64(dynstr, val, opts);
}

struct FormatF64Exp {
    i16 exp;
    f64 remaining;
};

static struct FormatF64Exp format_f64_decompose_exp(const f64 val, const FormatOptsFloat* opts) {
    static f64 binPow10[] = { 1e1, 1e2, 1e4, 1e8, 1e16, 1e32, 1e64, 1e128, 1e256 };
    static f64 negBinPow10[] = { 1e-1, 1e-2, 1e-4, 1e-8, 1e-16, 1e-32, 1e-64, 1e-128, 1e-256 };
    static f64 negBinPow10PlusOne[] = { 1e0, 1e-1, 1e-3, 1e-7, 1e-15, 1e-31, 1e-63, 1e-127, 1e-255 };

    struct FormatF64Exp res;
    res.exp = 0;
    res.remaining = val;

    i32 i = array_elems(binPow10) - 1;
    i32 bit = 1 << i;

    if (val >= opts->expThresholdPos) {
        for (; i >= 0; --i) {
            if (res.remaining >= binPow10[i]) {
                res.remaining *= negBinPow10[i];
                res.exp = (i16)(res.exp + bit);
            }

            bit >>= 1;
        }
    } else if (val > 0 && val <= opts->expThresholdNeg) {
        for (; i >= 0; --i) {
            if (res.remaining < negBinPow10PlusOne[i]) {
                res.remaining *= binPow10[i];
                res.exp = (i16)(res.exp - bit);
            }

            bit >>= 1;
        }
    }

    return res;
}

struct FormatF64Parts {
    u64 intPart;
    u64 decPart;
    u8 decDigits;
    i16 expPart;
};

static struct FormatF64Parts format_f64_decompose(const f64 val, const FormatOptsFloat* opts) {
    diag_assert(val >= 0.0);
    diag_assert(opts->minDecDigits <= opts->maxDecDigits);

    const struct FormatF64Exp exp = format_f64_decompose_exp(val, opts);

    struct FormatF64Parts res;
    res.expPart = exp.exp;
    res.decDigits = opts->maxDecDigits;
    res.intPart = (u64) exp.remaining;

    const u64 maxDecPart = math_pow10_u64(res.decDigits);
    f64 remainder = (exp.remaining - (f64)res.intPart) * (f64)maxDecPart;
    res.decPart = (u64)remainder;

    remainder -= res.decPart;
    if (remainder >= 0.5) {
        ++res.decPart;
        if (res.decPart >= maxDecPart) {
            res.decPart = 0;
            ++res.intPart;
            if (res.expPart && res.intPart >= 10) {
                res.expPart++;
                res.intPart = 1;
            }
        }
    }

    while (res.decPart % 10 == 0 && res.decDigits > opts->minDecDigits) {
        res.decPart /= 10;
        --res.decDigits;
    }

    return res;
}

void format_write_f64(DynString *dynstr, f64 val, const FormatOptsFloat *opts) {
    if (float_isnan(val)) {
        dynstring_append(dynstr, string_lit("nan"));

        return;
    }

    if (val < 0.0) {
        dynstring_append_char(dynstr, '-');
        val = -val;
    }

    if (float_isinf(val)) {
        dynstring_append(dynstr, string_lit("inf"));

        return;
    }

    const struct FormatF64Parts parts = format_f64_decompose(val, opts);

    format_write_int(dynstr, parts.intPart);
    if (parts.decDigits) {
        dynstring_append_char(dynstr, '.');
        format_write_int(dynstr, parts.decPart, .minDigits = parts.decDigits);
    }

    if (parts.expPart) {
        dynstring_append_char(dynstr, 'e');
        format_write_int(dynstr, parts.expPart);
    }
}

void format_write_bool(DynString *dynstr, bool val) {
    dynstring_append(dynstr, val ? string_lit("true") : string_lit("false"));
}

void format_write_bitset(DynString *dynstr, BitSet val) {
    for (usize i = bitset_size(val); i-- != 0; ) {
        dynstring_append_char(dynstr, bitset_test(val, i) ? '1' : '0');
    }
}

void format_write_mem(DynString *dynstr, Mem val) {
    diag_assert_msg(val.size <= usize_gibibyte, "Mem value too big: '{}'", fmt_size(val.size));
    for (usize i = val.size; i-- != 0; ) {
        format_write_int(dynstr, *mem_at_u8(val, i), .minDigits = 2, .base = 16);
    }
}

void format_write_time_duration_pretty(DynString *dynstr, TimeDuration val) {
    static struct {
        TimeDuration val;
        String str;
    } units[] = {
        { time_nanosecond, string_static("ns") },
        { time_microsecond, string_static("us") },
        { time_millisecond, string_static("ms") },
        { time_second, string_static("s") },
        { time_minute, string_static("m") },
        { time_hour, string_static("h") },
        { time_day, string_static("d") },
    };

    const TimeDuration absVal = math_abs(val);
    usize i = 0;
    for (; (i + 1) != array_elems(units) && absVal >= units[i + 1].val; ++i)
        ;

    format_write_float(dynstr, (f64)val / (f64)units[i].val, .maxDecDigits = 1);
    dynstring_append(dynstr, units[i].str);
}

void format_write_time_iso8601(DynString *dynstr, TimeReal val, const FormatOptsTime *opts) {
    const TimeReal localTime = time_real_offset(val, time_zone_to_duration(opts->timezone));
    const TimeDate date = time_real_to_date(localTime);
    const u8 hours = (localTime / (time_hour / time_microsecond)) % 24;
    const u8 minutes = (localTime / (time_minute / time_microsecond)) % 60;
    const u8 seconds = (localTime / (time_second / time_microsecond)) % 60;

    if (opts->terms & FormatTimeTerms_Date) {
        format_write_int(dynstr, date.year, .minDigits = 4);
        if (opts->flags & FormatTimeFlags_HumanReadable) {
            dynstring_append_char(dynstr, '-');
        }

        format_write_int(dynstr, date.month, .minDigits = 2);

        if (opts->flags & FormatTimeFlags_HumanReadable) {
            dynstring_append_char(dynstr, '-');
        }

        format_write_int(dynstr, date.day, .minDigits = 2);
    }

    if (opts->terms & FormatTimeTerms_Time) {
        dynstring_append_char(dynstr, 'T');
        format_write_int(dynstr, hours, .minDigits = 2);

        if (opts->flags & FormatTimeFlags_HumanReadable) {
            dynstring_append_char(dynstr, ':');
        }

        format_write_int(dynstr, minutes, .minDigits = 2);

        if (opts->flags & FormatTimeFlags_HumanReadable) {
            dynstring_append_char(dynstr, ':');
        }

        format_write_int(dynstr, seconds, .minDigits = 2);
    }

    if (opts->terms & FormatTimeTerms_Milliseconds) {
        const u16 milliseconds = (localTime / (time_millisecond / time_microsecond)) % 1000;

        if (opts->flags & FormatTimeFlags_HumanReadable) {
            dynstring_append_char(dynstr, '.');
        }

        format_write_int(dynstr, milliseconds, .minDigits = 3);
    }

    if (opts->terms & FormatTimeTerms_Timezone) {
        if (opts->timezone == time_zone_utc) {
            dynstring_append_char(dynstr, 'Z');
        } else {
            if (opts->timezone > 0) {
                dynstring_append_char(dynstr, '+');
            }

            format_write_int(dynstr, opts->timezone / 60, .minDigits = 2);

            if (opts->flags & FormatTimeFlags_HumanReadable) {
                dynstring_append_char(dynstr, ':');
            }

            format_write_int(dynstr, opts->timezone % 60, .minDigits = 2);
        }
    }
}

void format_write_size_pretty(DynString* dynstr, const usize val) {
    static String units[] = {
        string_static("B"),
        string_static("KiB"),
        string_static("MiB"),
        string_static("GiB"),
        string_static("TiB"),
        string_static("PiB"),
    };

    u8 unit = 0;
    f64 scaledSize = val;
    for (; scaledSize >= 1024.0 && unit != array_elems(units) - 1; ++unit) {
        scaledSize /= 1024.0;
    }

    format_write_float(dynstr, scaledSize, .maxDecDigits = 1);
    dynstring_append(dynstr, units[unit]);
}

void format_write_text(DynString *dynstr, String val, const FormatOptsText *opts) {
    diag_assert_msg(val.size <= usize_gibibyte, "Text too big: '{}'", fmt_size(val.size));
    mem_for_u8(val, byte, { format_write_char(dynstr, byte, opts); });
}

void format_write_text_wrapped(DynString* dynstr, String val, usize maxWidth, String linePrefix) {
    diag_assert_msg(maxWidth, "'maxWidth' of zero is not supported");
    diag_assert_msg(val.size <= usize_gibibyte, "Test too big: '{}'", fmt_size(val.size));

    usize column = 0;
    while (true) {
        while (!string_is_empty(val)) {
            switch (*string_begin(val)) {
                case '\r': {

                } break;

                case '\n': {
                    column = 0;
                    dynstring_append_char(dynstr, '\n');
                    dynstring_append(dynstr, linePrefix);
                } break;

                case '\t':
                case ' ': {
                    if (column >= maxWidth) {
                        column = 0;
                        dynstring_append_char(dynstr, '\n');
                        dynstring_append(dynstr, linePrefix);
                    } else {
                        dynstring_append_char(dynstr, ' ');
                        ++column;
                    }
                } break;

                default:
                    goto WhitespaceProcessed;
            }

            val = string_consume(val, 1);
        }

        WhitespaceProcessed:
            if (string_is_empty(val)) {
                break;
            }

            const usize wordEnd = string_find_first_any(val, string_lit("\r\n\t "));
            const String word = string_slice(val, 0, math_min(sentinel_check(wordEnd) ? val.size : wordEnd, maxWidth));

            if ((column + word.size) > maxWidth) {
                dynstring_append_char(dynstr, '\n');
                dynstring_append(dynstr, linePrefix);

                column = 0;
            }

            dynstring_append(dynstr, word);
            column += word.size;
            val = string_consume(val, word.size);
    }
}

void format_write_char(DynString *dynstr, const u8 val, const FormatOptsText *opts) {
    static struct {
        u8 byte;
        String escapeSeq;
    } escapes[] = {
        {'"', string_static("\\\"")},
        {'\\', string_static("\\\\")},
        {'\r', string_static("\\r")},
        {'\n', string_static("\\n")},
        {'\t', string_static("\\t")},
        {'\b', string_static("\\b")},
        {'\f', string_static("\\f")},
        {'\0', string_static("\\0")},
    };

    if (opts->flags & FormatTextFlags_EscapeNonPrintAscii && !ascii_is_printable(val)) {
        for (size_t i = 0; i != array_elems(escapes); ++i) {
            if (escapes[i].byte == val) {
                dynstring_append(dynstr, escapes[i].escapeSeq);
                return;
            }
        }

        dynstring_append_char(dynstr, '\\');
        format_write_int(dynstr, val, .base = 16, .minDigits = 2);
        return;
    }

    dynstring_append_char(dynstr, val);
}

String format_read_whitespace(const String input, String *output) {
    usize idx = 0;
    for (; idx != input.size && ascii_is_whitespace(*string_at(input, idx)); ++idx);
    if (output) {
        *output = string_slice(input, 0, idx);
    }

    return string_consume(input, idx);
}

static String format_read_sign(String input, i8* output) {
    i8 sign = 1;
    if (LIKELY(!string_is_empty(input))) {
        switch (*string_begin(input)) {
            case '-': {
                sign = -1;
            }

            case '+': {
                input = string_consume(input, 1);
            } break;
        }
    }

    if (LIKELY(output)) {
        *output = sign;
    }

    return input;
}

String format_read_u64(const String input, u64 *output, const u8 base) {
    usize idx = 0;
    u64 res = 0;
    for (; idx != input.size; ++idx) {
        const u8 val = ascii_to_integer(*string_at(input, idx));
        if (sentinel_check(val) || val >= base) {
            break;
        }

        res = res * base + val;
    }

    if (output) {
        *output = res;
    }

    return string_consume(input, idx);
}

String format_read_i64(String input, i64 *output, u8 base) {
    i8 sign;
    input = format_read_sign(input, &sign);

    u64 unsignedPart;
    const String rem = format_read_u64(input, &unsignedPart, base);
    if (output) {
        *output = (i64)unsignedPart * sign;
    }

    return rem;
}

String format_read_f64(String input, f64* output) {
    i8 sign;
    input = format_read_sign(input, &sign);

    f64 mantissa = 0.0;
    f64 divider = 1.0;
    bool passedDecPoint = false;

    while (!string_is_empty(input)) {
        const char ch = *string_begin(input);
        if (ch == '.' && !passedDecPoint) {
            passedDecPoint = true;
            input = string_consume(input, 1);

            continue;
        }

        if (!ascii_is_digit(ch)) {
            break;
        }

        mantissa = mantissa * 10.0 + ch - '0';
        if (passedDecPoint) {
            divider *= 10.0;
        }

        input = string_consume(input, 1);
    }

    if (!string_is_empty(input) && (*string_begin(input) == 'e' || *string_begin(input) == 'E')) {
        i64 exp = 0;
        input = format_read_i64(string_consume(input, 1), &exp, 10);

        if (exp >= 0) {
            divider /= math_pow10_u64(math_min((u8)exp, 19));
        } else {
            divider *= math_pow10_u64(math_min((u8)-exp, 19));
        }
    }

    if (output) {
        *output = mantissa / divider * (f64)sign;
    }

    return input;
}
