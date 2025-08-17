/**
 * @file failure.c
 * @brief Implementation of CLI parsing failure reporting and error formatting
 *
 * This file implements functionality for collecting, formatting, and displaying
 * CLI parsing errors with optional terminal styling support. Provides utilities
 * for writing error messages to strings or files with appropriate formatting.
 */

#include "core_dynstring.h"
#include "core_format.h"
#include "core_tty.h"

#include "cli_failure.h"
#include "cli_parse.h"

/**
 * @brief Generate red background style format argument if styling is enabled
 *
 * Returns a TTY style format argument with red background and bold text
 * if the Style flag is set, otherwise returns a no-op format argument.
 *
 * @param flags Failure formatting flags
 * @return Format argument for red background styling or no-op
 */
static FormatArg arg_style_red_bg(const CliFailureFlags flags) {
    return flags & CliFailureFlags_Style ? fmt_ttystyle(.bgColor = TtyBgColor_Red, .flags = TtyStyleFlags_Bold) : fmt_nop();
}

/**
 * @brief Generate style reset format argument if styling is enabled
 *
 * Returns a TTY style reset format argument to clear all styling
 * if the Style flag is set, otherwise returns a no-op format argument.
 *
 * @param flags Failure formatting flags
 * @return Format argument for style reset or no-op
 */
static FormatArg arg_style_reset(const CliFailureFlags flags) {
    return flags & CliFailureFlags_Style ? fmt_ttystyle() : fmt_nop();
}

/**
 * @brief Write parsing failure messages to a dynamic string
 *
 * Retrieves all parsing errors from the invocation and formats them
 * into the provided dynamic string with optional styling.
 *
 * @param dynstr Target dynamic string for output
 * @param invoc CLI invocation containing parsing errors
 * @param flags Formatting flags controlling styling options
 */
void cli_failure_write(DynString* dynstr, CliInvocation* invoc, const CliFailureFlags flags) {
    CliParseErrors errors = cli_parse_errors(invoc);

    for (String* err = errors.head; err != errors.head + errors.count; ++err) {
        fmt_write(dynstr, "{}{}{}\n", arg_style_red_bg(flags), fmt_text(*err), arg_style_reset(flags));
    }
}

/**
 * @brief Write parsing failure messages to a file
 *
 * Creates a temporary dynamic string, formats all parsing errors into it
 * with appropriate styling based on TTY detection, then writes the result
 * to the specified file.
 *
 * @param invoc CLI invocation containing parsing errors
 * @param out Output file to write error messages to
 */
void cli_failure_write_file(CliInvocation* invoc, File* out) {
    DynString str = dynstring_create(g_alloc_heap, 512);

    const CliFailureFlags flags = tty_isatty(out) ? CliFailureFlags_Style : CliFailureFlags_None;

    cli_failure_write(&str, invoc, flags);

    file_write_sync(out, dynstring_view(&str));
    dynstring_destroy(&str);
}