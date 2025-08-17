/**
 * @file cli_read.h
 * @brief CLI value reading and conversion functions
 *
 * This header provides convenience functions for reading and converting parsed
 * command-line argument values to specific data types. Each function attempts
 * to extract and convert the first value associated with a CLI option or argument,
 * returning a default value if the option was not provided or conversion fails.
 */

#pragma once

#include "core_string.h"
#include "core_types.h"

/**
 * @brief Forward declaration of CliInvocation structure
 */
typedef struct sCliInvocation CliInvocation;

/**
 * @brief Unique identifier type for CLI options and arguments
 */
typedef u16 CliId;

/**
 * @brief Read a string value from a CLI option or argument
 *
 * @param invocation The CLI invocation to read from
 * @param id The ID of the option or argument
 * @param defaultVal The default value to return if the option was not provided
 * @return The string value if available, otherwise the default value
 */
String cli_read_string(CliInvocation*, CliId, String defaultVal);

/**
 * @brief Read a signed 64-bit integer value from a CLI option or argument
 *
 * @param invocation The CLI invocation to read from
 * @param id The ID of the option or argument
 * @param defaultVal The default value to return if the option was not provided or conversion fails
 * @return The integer value if available and valid, otherwise the default value
 */
i64 cli_read_i64(CliInvocation*, CliId, i64 defaultVal);

/**
 * @brief Read an unsigned 64-bit integer value from a CLI option or argument
 *
 * @param invocation The CLI invocation to read from
 * @param id The ID of the option or argument
 * @param defaultVal The default value to return if the option was not provided or conversion fails
 * @return The unsigned integer value if available and valid, otherwise the default value
 */
u64 cli_read_u64(CliInvocation*, CliId, u64 defaultVal);

/**
 * @brief Read a 64-bit floating-point value from a CLI option or argument
 *
 * @param invocation The CLI invocation to read from
 * @param id The ID of the option or argument
 * @param defaultVal The default value to return if the option was not provided or conversion fails
 * @return The floating-point value if available and valid, otherwise the default value
 */
f64 cli_read_f64(CliInvocation*, CliId, f64 defaultVal);