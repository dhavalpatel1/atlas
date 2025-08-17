/**
 * @file cli_help.h
 * @brief CLI help system for generating usage and option documentation
 *
 * This header provides functionality for generating formatted help text that
 * describes the usage, options, and arguments of a CLI application. It supports
 * both dynamic string output and direct file output, with optional styling.
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
 * @brief Flags controlling the formatting of help output
 */
typedef enum {
    CliHelpFlags_None = 0,        /**< No special formatting */
    CliHelpFlags_Style = 1 << 0   /**< Apply styling to the output */
} CliHelpFlags;

/**
 * @brief Write help information to a dynamic string
 *
 * Generates formatted help text including usage information, option descriptions,
 * and argument details for the specified CLI application.
 *
 * @param output The dynamic string to append the help text to
 * @param app The CLI application to generate help for
 * @param flags Flags controlling the output formatting
 */
void cli_help_write(DynString*, CliApp*, CliHelpFlags);

/**
 * @brief Write help information directly to a file
 *
 * @param app The CLI application to generate help for
 * @param out The file to write the help text to
 */
void cli_help_write_file(CliApp*, File* out);