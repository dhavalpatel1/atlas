/**
 * @file validate.c
 * @brief Implementation of CLI option value validation functions
 *
 * This file implements a collection of validation functions for common data types
 * that can be used with CLI options. These validators ensure that string inputs
 * can be successfully parsed as the expected data type.
 */

#include "core_format.h"

#include "cli_validate.h"

/**
 * @brief Validate that a string can be parsed as a signed 64-bit integer
 *
 * Attempts to parse the input string as a base-10 signed 64-bit integer
 * and returns whether the entire string was consumed (valid).
 *
 * @param input The string to validate
 * @return True if the string is a valid signed 64-bit integer, false otherwise
 */
bool cli_validate_i64(const String input) {
    const u8 base = 10;
    const String rem = format_read_i64(input, null, base);

    return string_is_empty(rem);
}

/**
 * @brief Validate that a string can be parsed as an unsigned 64-bit integer
 *
 * Attempts to parse the input string as a base-10 unsigned 64-bit integer
 * and returns whether the entire string was consumed (valid).
 *
 * @param input The string to validate
 * @return True if the string is a valid unsigned 64-bit integer, false otherwise
 */
bool cli_validate_u64(const String input) {
    const u8 base = 10;
    const String rem = format_read_u64(input, null, base);

    return string_is_empty(rem);
}

/**
 * @brief Validate that a string can be parsed as a 64-bit floating point number
 *
 * Attempts to parse the input string as a 64-bit floating point number
 * and returns whether the entire string was consumed (valid).
 *
 * @param input The string to validate
 * @return True if the string is a valid 64-bit floating point number, false otherwise
 */
bool cli_validate_f64(const String input) {
    const String rem = format_read_f64(input, null);

    return string_is_empty(rem);
}