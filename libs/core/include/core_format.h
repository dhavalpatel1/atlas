/**
 * @file core_format.h
 * @brief String formatting utilities for type-safe text output
 *
 * This header provides a comprehensive string formatting system with type-safe
 * argument handling. It supports formatted output of various data types including
 * integers, floats, text, time, memory, and bitsets. The system uses variadic
 * macros and type-generic selection to provide a printf-like interface with
 * better type safety and extensibility.
 */

#pragma once

#include "core_bitset.h"
#include "core_dynstring.h"
#include "core_macro.h"
#include "core_time.h"
#include "core_tty.h"
#include "core_types.h"

/**
 * @brief Enumeration of format argument types
 */
typedef enum {
    FormatArgType_End,           /**< End of argument list marker */
    FormatArgType_Nop,           /**< No-operation argument */
    FormatArgType_List,          /**< List of arguments */
    FormatArgType_i64,           /**< Signed 64-bit integer */
    FormatArgType_u64,           /**< Unsigned 64-bit integer */
    FormatArgType_f64,           /**< 64-bit floating point */
    FormatArgType_bool,          /**< Boolean value */
    FormatArgType_BitSet,        /**< Bitset value */
    FormatArgType_Mem,           /**< Memory region */
    FormatArgType_Duration,      /**< Time duration */
    FormatArgType_Time,          /**< Time value */
    FormatArgType_Size,          /**< Size value */
    FormatArgType_Text,          /**< Text string */
    FormatArgType_Char,          /**< Single character */
    FormatArgType_Path,          /**< File path */
    FormatArgType_TtyStyle,      /**< Terminal styling */
    FormatArgType_Padding        /**< Padding amount */
} FormatArgType;

/** @brief Forward declaration of format argument structure */
typedef struct sFormatArg FormatArg;

/**
 * @brief Structure representing a typed format argument
 */
struct sFormatArg {
    FormatArgType type;          /**< Type of the argument */
    union {
        const FormatArg* value_list;     /**< List of child arguments */
        i64 value_i64;                   /**< Signed 64-bit integer value */
        u64 value_u64;                   /**< Unsigned 64-bit integer value */
        f64 value_f64;                   /**< 64-bit floating point value */
        bool value_bool;                 /**< Boolean value */
        BitSet value_bitset;             /**< Bitset value */
        Mem value_mem;                   /**< Memory region value */
        TimeDuration value_duration;     /**< Time duration value */
        TimeReal value_time;             /**< Time value */
        usize value_size;                /**< Size value */
        String value_text;               /**< Text string value */
        u8 value_char;                   /**< Single character value */
        String value_path;               /**< File path value */
        TtyStyle value_ttystyle;         /**< Terminal styling value */
        u16 value_padding;               /**< Padding amount value */
    };
    void* settings;              /**< Type-specific formatting settings */
};

/**
 * @brief Create a no-operation format argument
 * @return FormatArg representing a no-operation
 */
#define fmt_nop()                 \
    ((FormatArg) {                \
        .type = FormatArgType_Nop \
    })

/**
 * @brief Create an end-of-arguments marker
 * @return FormatArg representing the end of an argument list
 */
#define fmt_end() ((FormatArg){0})

/**
 * @brief Create a list format argument from an array of child arguments
 * @param _CHILD_ARGS_ Array of FormatArg to include in the list
 * @param ... Optional list formatting options
 * @return FormatArg representing a formatted list
 */
#define fmt_list(_CHILD_ARGS_, ...)                \
    ((FormatArg) {                                 \
        .type = FormatArgType_List,                \
        .value_list = (_CHILD_ARGS_),              \
        .settings = &format_opts_list(__VA_ARGS__) \
    })

/**
 * @brief Create a list format argument from literal arguments
 * @param ... FormatArg literals to include in the list
 * @return FormatArg representing a formatted list
 */
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

/**
 * @brief Options for formatting lists
 */
typedef struct {
    String prefix;      /**< String to prepend to the list */
    String suffix;      /**< String to append to the list */
    String seperator;   /**< String to use between list items */
} FormatOptsList;

/**
 * @brief Options for formatting integers
 */
typedef struct {
    u8 base;        /**< Number base (2, 8, 10, 16, etc.) */
    u8 minDigits;   /**< Minimum number of digits (pad with zeros) */
} FormatOptsInt;

/**
 * @brief Options for formatting floating-point numbers
 */
typedef struct {
    u8 minDecDigits;        /**< Minimum decimal digits to display */
    u8 maxDecDigits;        /**< Maximum decimal digits to display */
    f64 expThresholdPos;    /**< Threshold for scientific notation (positive) */
    f64 expThresholdNeg;    /**< Threshold for scientific notation (negative) */
} FormatOptsFloat;

/**
 * @brief Flags for specifying which time components to include
 */
typedef enum {
    FormatTimeTerms_None         = 0,        /**< No time components */
    FormatTimeTerms_Date         = 1 << 0,   /**< Include date */
    FormatTimeTerms_Time         = 1 << 1,   /**< Include time */
    FormatTimeTerms_Milliseconds = 1 << 2,   /**< Include milliseconds */
    FormatTimeTerms_Timezone     = 1 << 3,   /**< Include timezone */
    FormatTimeTerms_All          = ~FormatTimeTerms_None /**< Include all components */
} FormatTimeTerms;

/**
 * @brief Options for formatting time values
 */
typedef struct {
    TimeZone timezone;      /**< Timezone for time conversion */
    FormatTimeTerms terms;  /**< Which time components to include */
} FormatOptsTime;

/**
 * @brief Flags for text formatting options
 */
typedef enum {
    FormatTextFlags_None = 0,                      /**< No special text formatting */
    FormatTextFlags_EscapeNonPrintAscii = 1 << 0,  /**< Escape non-printable ASCII characters */
} FormatTextFlags;

/**
 * @brief Options for formatting text strings
 */
typedef struct {
    FormatTextFlags flags;  /**< Text formatting flags */
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

/**
 * @brief Write a single format argument to a dynamic string
 * @param dynstr Dynamic string to write to
 * @param arg Format argument to write
 */
void format_write_arg(DynString* dynstr, const FormatArg* arg);

/**
 * @brief Write formatted text using a format string and arguments
 * @param dynstr Dynamic string to write to
 * @param format Format string template
 * @param args Array of format arguments
 */
void format_write_formatted(DynString* dynstr, String format, const FormatArg* args);

/**
 * @brief Write formatted text using scratch allocator
 * @param format Format string template
 * @param args Array of format arguments
 * @return Formatted string allocated with scratch allocator
 */
String format_write_formatted_scratch(String format, const FormatArg* args);

/**
 * @brief Write an unsigned 64-bit integer to a dynamic string
 * @param dynstr Dynamic string to write to
 * @param val Integer value to write
 * @param opts Integer formatting options
 */
void format_write_u64(DynString* dynstr, u64 val, const FormatOptsInt* opts);

/**
 * @brief Write a signed 64-bit integer to a dynamic string
 * @param dynstr Dynamic string to write to
 * @param val Integer value to write
 * @param opts Integer formatting options
 */
void format_write_i64(DynString* dynstr, i64 val, const FormatOptsInt* opts);

/**
 * @brief Write a 64-bit floating-point number to a dynamic string
 * @param dynstr Dynamic string to write to
 * @param val Float value to write
 * @param opts Float formatting options
 */
void format_write_f64(DynString* dynstr, f64 val, const FormatOptsFloat* opts);

/**
 * @brief Write a boolean value to a dynamic string
 * @param dynstr Dynamic string to write to
 * @param val Boolean value to write
 */
void format_write_bool(DynString* dynstr, bool val);

/**
 * @brief Write a bitset to a dynamic string
 * @param dynstr Dynamic string to write to
 * @param val Bitset value to write
 */
void format_write_bitset(DynString* dynstr, BitSet val);

/**
 * @brief Write a memory region to a dynamic string
 * @param dynstr Dynamic string to write to
 * @param val Memory region to write
 */
void format_write_mem(DynString* dynstr, Mem val);

/**
 * @brief Write a time duration in human-readable format
 * @param dynstr Dynamic string to write to
 * @param val Time duration to write
 */
void format_write_time_duration_pretty(DynString* dynstr, TimeDuration val);

/**
 * @brief Write a time value in ISO 8601 format
 * @param dynstr Dynamic string to write to
 * @param val Time value to write
 * @param opts Time formatting options
 */
void format_write_time_iso8601(DynString* dynstr, TimeReal val, const FormatOptsTime* opts);

/**
 * @brief Write a size value in human-readable format (bytes, KB, MB, etc.)
 * @param dynstr Dynamic string to write to
 * @param val Size value to write
 */
void format_write_size_pretty(DynString* dynstr, usize val);

/**
 * @brief Write a text string with optional formatting
 * @param dynstr Dynamic string to write to
 * @param val Text string to write
 * @param opts Text formatting options
 */
void format_write_text(DynString* dynstr, String val, const FormatOptsText* opts);

/**
 * @brief Write text with word wrapping and line prefixes
 * @param dynstr Dynamic string to write to
 * @param val Text string to write
 * @param maxWidth Maximum line width for wrapping
 * @param linePrefix Prefix to add to each line
 */
void format_write_text_wrapped(DynString* dynstr, String val, usize maxWidth, String linePrefix);

/**
 * @brief Write a single character with optional formatting
 * @param dynstr Dynamic string to write to
 * @param val Character to write
 * @param opts Text formatting options
 */
void format_write_char(DynString* dynstr, u8 val, const FormatOptsText* opts);

/**
 * @brief Parse whitespace from input string
 * @param input Input string to parse from
 * @param output Pointer to store the parsed whitespace
 * @return Remaining input string after parsing
 */
String format_read_whitespace(String input, String* output);

/**
 * @brief Parse an unsigned 64-bit integer from input string
 * @param input Input string to parse from
 * @param output Pointer to store the parsed integer
 * @param base Number base for parsing
 * @return Remaining input string after parsing
 */
String format_read_u64(String input, u64* output, u8 base);

/**
 * @brief Parse a signed 64-bit integer from input string
 * @param input Input string to parse from
 * @param output Pointer to store the parsed integer
 * @param base Number base for parsing
 * @return Remaining input string after parsing
 */
String format_read_i64(String input, i64* output, u8 base);

/**
 * @brief Parse a 64-bit floating-point number from input string
 * @param input Input string to parse from
 * @param output Pointer to store the parsed float
 * @return Remaining input string after parsing
 */
String format_read_f64(String input, f64* output);