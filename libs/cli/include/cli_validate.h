/**
 * @file cli_validate.h
 * @brief CLI input validation functions and utilities
 *
 * This header provides built-in validation functions for common data types
 * that can be used with CLI options and arguments. These validators check
 * whether input strings can be successfully converted to specific numeric
 * types, helping ensure data integrity before processing.
 */

#pragma once

#include "core_string.h"
#include "core_types.h"

/**
 * @brief Function pointer type for validating CLI input values
 *
 * @param input The input string to validate
 * @return true if the input is valid, false otherwise
 */
typedef bool (*CliValidateFunc)(const String input);

/**
 * @brief Validate that a string can be converted to a signed 64-bit integer
 *
 * @param input The input string to validate
 * @return true if the string represents a valid signed 64-bit integer, false otherwise
 */
bool cli_validate_i64(const String input);

/**
 * @brief Validate that a string can be converted to an unsigned 64-bit integer
 *
 * @param input The input string to validate
 * @return true if the string represents a valid unsigned 64-bit integer, false otherwise
 */
bool cli_validate_u64(const String input);

/**
 * @brief Validate that a string can be converted to a 64-bit floating-point number
 *
 * @param input The input string to validate
 * @return true if the string represents a valid 64-bit floating-point number, false otherwise
 */
bool cli_validate_f64(const String input);