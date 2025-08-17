/**
 * @file cli_failure.h
 * @brief CLI error handling and failure reporting functions
 *
 * This header provides functionality for generating and outputting error messages
 * when CLI parsing fails. It supports both formatted string output and direct
 * file output, with optional styling for enhanced readability.
 */

#pragma once

#include "core_dynstring.h"

/**
 * @brief Forward declaration of File structure
 */
typedef struct sFile File;

/**
 * @brief Forward declaration of CliApp structure
 */
typedef struct sCliApp CliApp;

/**
 * @brief Forward declaration of CliInvocation structure
 *
 * Represents the result of parsing command-line arguments, including
 * any errors that occurred during parsing.
 */
typedef struct sCliInvocation CliInvocation;

/**
 * @brief Flags controlling the formatting of failure output
 */
typedef enum {
    CliFailureFlags_None = 0,        /**< No special formatting */
    CliFailureFlags_Style = 1 << 0   /**< Apply styling to the output */
} CliFailureFlags;

/**
 * @brief Write failure information to a dynamic string
 *
 * @param output The dynamic string to append the failure message to
 * @param invocation The CLI invocation containing error information
 * @param flags Flags controlling the output formatting
 */
void cli_failure_write(DynString*, CliInvocation*, CliFailureFlags);

/**
 * @brief Write failure information directly to a file
 *
 * @param invocation The CLI invocation containing error information
 * @param file The file to write the failure message to
 */
void cli_failure_write_file(CliInvocation*, File*);