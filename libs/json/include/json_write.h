/**
 * @file json_write.h
 * @brief JSON serialization functionality for converting JSON documents to text
 *
 * This header provides JSON writing capabilities that convert JSON document
 * structures into formatted text strings. Supports both compact and pretty-printed
 * output formats with customizable indentation and line endings.
 */

#pragma once

#include "core_dynstring.h"
#include "json_doc.h"

/**
 * @brief JSON writing flags enumeration
 */
typedef enum {
    JsonWriteFlags_None = 0,            /**< No special formatting */
    JsonWriteFlags_Pretty = 1 << 0,     /**< Enable pretty-printing with indentation */
} JsonWriteFlags;

/**
 * @brief JSON writing options structure
 */
typedef struct {
    JsonWriteFlags flags;       /**< Writing flags controlling output format */
    String indent;              /**< String to use for each indentation level */
    String newline;             /**< String to use for line endings */
} JsonWriteOpts;

/**
 * @brief Create JSON write options with pretty-printing defaults
 * @param ... Additional options to override defaults
 * @return JsonWriteOpts structure with specified settings
 *
 * Default settings:
 * - Pretty-printing enabled
 * - 2-space indentation
 * - Unix-style line endings (\n)
 *
 * Example usage:
 * @code
 * JsonWriteOpts opts = json_write_opts(.indent = string_lit("\t"));
 * @endcode
 */
#define json_write_opts(...)            \
    ((JsonWriteOpts){                   \
        .flags = JsonWriteFlags_Pretty, \
        .indent = string_lit("  "),     \
        .newline = string_lit("\n"),    \
        __VA_ARGS__                     \
    })

/**
 * @brief Write a JSON value to a dynamic string
 * @param output Dynamic string to write the JSON text to
 * @param doc JSON document containing the value
 * @param val JSON value handle to write
 * @param opts Writing options controlling output format (null for compact output)
 *
 * Converts the specified JSON value and all its children into a properly
 * formatted JSON text string. The output format is controlled by the options:
 * - null opts: compact output with no extra whitespace
 * - Pretty flag: formatted output with indentation and line breaks
 */
void json_write(DynString* output, const JsonDoc* doc, JsonVal val, const JsonWriteOpts* opts);