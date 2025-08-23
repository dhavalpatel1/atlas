#pragma once

#include "core_bitset.h"
#include "core_dynstring.h"
#include "core_macro.h"
#include "core_time.h"
#include "core_tty.h"
#include "core_types.h"

typedef enum {
    FormatArgType_End,
    FormatArgType_Nop,
    FormatArgType_List,
    FormatArgType_i64,
    FormatArgType_u64,
    FormatArgType_f64,
    FormatArgType_bool,
    FormatArgType_BitSet,
    FormatArgType_Mem,
    FormatArgType_Duration,
    FormatArgType_Time,
    FormatArgType_Size,
    FormatArgType_Text,
    FormatArgType_Char,
    FormatArgType_Path,
    FormatArgType_TtyStyle,
    FormatArgType_Padding
} FormatArgType;

typedef struct sFormatArg FormatArg;

struct sFormatArg {
    FormatArgType type;
    union {
        const FormatArg* value_list;
        i64 value_i64;
        u64 value_u64;
        f64 value_f64;
        bool value_bool;
        BitSet value_bitset;
        Mem value_mem;
        TimeDuration value_duration;
        TimeReal value_time;
        usize value_size;
        String value_text;
        u8 value_char;
        String value_path;
        TtyStyle value_ttystyle;
        u16 value_padding;
    };
    void* settings;
};

#define fmt_nop()                 \
    ((FormatArg) {                \
        .type = FormatArgType_Nop \
    })

#define fmt_end() ((FormatArg){0})

#define fmt_list(_CHILD_ARGS_, ...)                \
    ((FormatArg) {                                 \
        .type = FormatArgType_List,                \
        .value_list = (_CHILD_ARGS_),              \
        .settings = &format_opts_list(__VA_ARGS__) \
    })

#define fmt_list_lit(...)                                                                         \
    ((FormatArg) {                                                                                \
        .type = FormatArgType_List,                                                               \
        .value_list = (const FormatArg[]){ VA_ARGS_SKIP_FIRST(0, ##__VA_ARGS__, (FormatArg){0})}, \
        .settings = &format_opts_list()                                                           \
    })

#define fmt_int(_VAL_, ...)                           \
    _Generic(+(_VAL_),                                \
        u32: ((FormatArg) {                           \
            .type = FormatArgType_u64,                \
            .value_u64 = (u64)(_VAL_),                \
            .settings = &format_opts_int(__VA_ARGS__) \
        }),                                           \
        i32: ((FormatArg) {                           \
            .type = FormatArgType_i64,                \
            .value_i64 = (i64)(_VAL_),                \
            .settings = &format_opts_int(__VA_ARGS__) \
        }),                                           \
        u64: ((FormatArg) {                           \
            .type = FormatArgType_u64,                \
            .value_u64 = (u64)(_VAL_),                \
            .settings = &format_opts_int(__VA_ARGS__) \
        }),                                           \
        i64: ((FormatArg) {                           \
            .type = FormatArgType_i64,                \
            .value_i64 = (i64)(_VAL_),                \
            .settings = &format_opts_int(__VA_ARGS__) \
        })                                            \
)

#define fmt_float(_VAL_, ...)                           \
    ((FormatArg) {                                      \
        .type = FormatArgType_f64,                      \
        .value_f64 = (_VAL_),                           \
        .settings = &format_opts_float(__VA_ARGS__)     \
    })                                                  

#define fmt_bool(_VAL_)             \
    ((FormatArg) {                  \
        .type = FormatArgType_bool, \
        .value_bool = (_VAL_)       \
    })

#define fmt_bitset(_VAL_)             \
    ((FormatArg) {                    \
        .type = FormatArgType_BitSet, \
        .value_bitset = (_VAL_)       \
    })

#define fmt_mem(_VAL_)             \
    ((FormatArg) {                 \
        .type = FormatArgType_Mem, \
        .value_mem = (_VAL_)       \
    })

#define fmt_size(_VAL_)             \
    ((FormatArg) {                  \
        .type = FormatArgType_Size, \
        .value_size = (_VAL_)       \
    })

#define fmt_duration(_VAL_)             \
    ((FormatArg) {                      \
        .type = FormatArgType_Duration, \
        .value_duration = (_VAL_)       \
    })

#define fmt_time(_VAL_, ...)                       \
    ((FormatArg) {                                 \
        .type = FormatArgType_Time,                \
        .value_time = (_VAL_),                     \
        .settings = &format_opts_time(__VA_ARGS__) \
    })

#define fmt_text(_VAL_, ...) ((FormatArg) {        \
        .type = FormatArgType_Text,                \
        .value_text = (_VAL_),                     \
        .settings = &format_opts_text(__VA_ARGS__) \
    })

#define fmt_text_lit(_VAL_) fmt_text(string_lit(_VAL_))

#define fmt_char(_VAL_, ...) ((FormatArg) {        \
        .type = FormatArgType_Char,                \
        .value_char = (_VAL_),                     \
        .settings = &format_opts_text(__VA_ARGS__) \
    })

#define fmt_path(_VAL_) ((FormatArg) { \
        .type = FormatArgType_Path,    \
        .value_path = (_VAL_)          \
    })

#define fmt_ttystyle(...) ((FormatArg) {          \
        .type = FormatArgType_TtyStyle,           \
        .value_ttystyle = (ttystyle(__VA_ARGS__)) \
    })

 #define fmt_padding(_AMOUNT_) ((FormatArg) { \
        .type = FormatArgType_Padding,        \
        .value_padding = (_AMOUNT_)           \
    })

#define fmt_args(...) (const FormatArg[]) {              \
    VA_ARGS_SKIP_FIRST(0, ##__VA_ARGS__, (FormatArg){0}) \
}

#define fmt_write(_DYNSTRING_, _FORMAT_LIT_, ...) format_write_formatted((_DYNSTRING_), string_lit(_FORMAT_LIT_), fmt_args(__VA_ARGS__))

#define fmt_write_scratch(_FORMAT_LIT_, ...) format_write_formatted_scratch(string_lit(_FORMAT_LIT_), fmt_args(__VA_ARGS__))

typedef struct {
    String prefix;
    String suffix;
    String seperator;
} FormatOptsList;

typedef struct {
    u8 base;
    u8 minDigits;
} FormatOptsInt;

typedef struct {
    u8 minDecDigits;
    u8 maxDecDigits;
    f64 expThresholdPos;
    f64 expThresholdNeg;
} FormatOptsFloat;

typedef enum {
    FormatTimeTerms_None         = 0,
    FormatTimeTerms_Date         = 1 << 0,
    FormatTimeTerms_Time         = 1 << 1,
    FormatTimeTerms_Milliseconds = 1 << 2,
    FormatTimeTerms_Timezone     = 1 << 3,
    FormatTimeTerms_All          = ~FormatTimeTerms_None
} FormatTimeTerms;

typedef struct {
    TimeZone timezone;
    FormatTimeTerms terms;
} FormatOptsTime;

typedef enum {
    FormatTextFlags_None = 0,
    FormatTextFlags_EscapeNonPrintAscii = 1 << 0,
} FormatTextFlags;

typedef struct {
    FormatTextFlags flags;
} FormatOptsText;

#define format_opts_list(...)          \
    ((FormatOptsList) {                \
        .prefix = string_empty,        \
        .suffix = string_empty,        \
        .seperator = string_lit(", "), \
        __VA_ARGS__                    \
    })

#define format_opts_int(...) \
    ((FormatOptsInt) {       \
        .base = 10,          \
        .minDigits = 0,      \
        __VA_ARGS__          \
    })

#define format_opts_float(...)   \
    ((FormatOptsFloat) {         \
        .minDecDigits = 0,       \
        .maxDecDigits = 7,       \
        .expThresholdPos = 1e7,  \
        .expThresholdNeg = 1e-5, \
        __VA_ARGS__              \
    })

#define format_opts_time(...)         \
    ((FormatOptsTime) {               \
        .timezone = time_zone_utc,    \
        .terms = FormatTimeTerms_All, \
        __VA_ARGS__                   \
    })

#define format_opts_text(...)          \
    ((FormatOptsText) {                \
        .flags = FormatTextFlags_None, \
        __VA_ARGS__                    \
    })

#define format_write_int(_DYNSTRING_, _VAL_, ...)                                        \
    _Generic(+(_VAL_),                                                                   \
        u32: format_write_u64(_DYNSTRING_, (u64)(_VAL_), &format_opts_int(__VA_ARGS__)), \
        i32: format_write_i64(_DYNSTRING_, (i64)(_VAL_), &format_opts_int(__VA_ARGS__)), \
        u64: format_write_u64(_DYNSTRING_, _VAL_, &format_opts_int(__VA_ARGS__)),        \
        i64: format_write_i64(_DYNSTRING_, _VAL_, &format_opts_int(__VA_ARGS__))         \
)

#define format_write_float(_DYNSTRING_, _VAL_, ...)                           \
        format_write_f64(_DYNSTRING_, _VAL_, &format_opts_float(__VA_ARGS__)) \

void format_write_arg(DynString* dynstr, const FormatArg* arg);

void format_write_formatted(DynString* dynstr, String format, const FormatArg* args);

String format_write_formatted_scratch(String format, const FormatArg* args);

void format_write_u64(DynString* dynstr, u64 val, const FormatOptsInt* opts);

void format_write_i64(DynString* dynstr, i64 val, const FormatOptsInt* opts);

void format_write_f64(DynString* dynstr, f64 val, const FormatOptsFloat* opts);

void format_write_bool(DynString* dynstr, bool val);

void format_write_bitset(DynString* dynstr, BitSet val);

void format_write_mem(DynString* dynstr, Mem val);

void format_write_time_duration_pretty(DynString* dynstr, TimeDuration val);

void format_write_time_iso8601(DynString* dynstr, TimeReal val, const FormatOptsTime* opts);

void format_write_size_pretty(DynString* dynstr, usize val);

void format_write_text(DynString* dynstr, String val, const FormatOptsText* opts);

void format_write_text_wrapped(DynString* dynstr, String val, usize maxWidth, String linePrefix);

void format_write_char(DynString* dynstr, u8 val, const FormatOptsText* opts);

String format_read_whitespace(String input, String* output);

String format_read_u64(String input, u64* output, u8 base);

String format_read_i64(String input, i64* output, u8 base);

String format_read_f64(String input, f64* output);