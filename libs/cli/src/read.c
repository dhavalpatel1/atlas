/**
 * @file read.c
 * @brief Implementation of CLI option value reading and type conversion
 *
 * This file implements utility functions for extracting and converting option
 * values from parsed CLI invocations. Provides type-safe accessors for common
 * data types with default value support for missing options.
 */

#include "core_format.h"

#include "cli_parse.h"
#include "cli_read.h"

/**
 * @brief Read a string value from an option
 *
 * Retrieves the first string value for the specified option, or returns
 * the default value if the option was not provided.
 *
 * @param invoc The CLI invocation
 * @param id The option ID
 * @param defaultVal Default value to return if option not provided
 * @return The option's string value or the default value
 */
String cli_read_string(CliInvocation* invoc, const CliId id, const String defaultVal) {
    CliParseValues values = cli_parse_values(invoc, id);

    return values.count ? *values.head : defaultVal;
}

/**
 * @brief Read a signed 64-bit integer value from an option
 *
 * Retrieves and parses the first value for the specified option as a
 * signed 64-bit integer, or returns the default value if not provided.
 *
 * @param invoc The CLI invocation
 * @param id The option ID
 * @param defaultVal Default value to return if option not provided
 * @return The option's integer value or the default value
 */
i64 cli_read_i64(CliInvocation* invoc, const CliId id, const i64 defaultVal) {
    CliParseValues values = cli_parse_values(invoc, id);
    if (!values.count) {
        return defaultVal;
    }

    i64 result;
    const u8 base = 10;
    format_read_i64(values.head[0], &result, base);

    return result;
}

/**
 * @brief Read an unsigned 64-bit integer value from an option
 *
 * Retrieves and parses the first value for the specified option as an
 * unsigned 64-bit integer, or returns the default value if not provided.
 *
 * @param invoc The CLI invocation
 * @param id The option ID
 * @param defaultVal Default value to return if option not provided
 * @return The option's integer value or the default value
 */
u64 cli_read_u64(CliInvocation* invoc, const CliId id, const u64 defaultVal) {
    CliParseValues values = cli_parse_values(invoc, id);
    if (!values.count) {
        return defaultVal;
    }

    u64 result;
    const u8 base = 10;
    format_read_u64(values.head[0], &result, base);

    return result;
}

/**
 * @brief Read a 64-bit floating point value from an option
 *
 * Retrieves and parses the first value for the specified option as a
 * 64-bit floating point number, or returns the default value if not provided.
 *
 * @param invoc The CLI invocation
 * @param id The option ID
 * @param defaultVal Default value to return if option not provided
 * @return The option's floating point value or the default value
 */
f64 cli_read_f64(CliInvocation* invoc, const CliId id, const f64 defaultVal) {
    CliParseValues values = cli_parse_values(invoc, id);
    if (!values.count) {
        return defaultVal;
    }

    f64 result;
    format_read_f64(values.head[0], &result);

    return result;
}