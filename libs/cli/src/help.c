/**
 * @file help.c
 * @brief Implementation of CLI help text generation and formatting
 *
 * This file implements functionality for generating comprehensive help documentation
 * for CLI applications including usage lines, argument descriptions, flag descriptions,
 * and formatted output with optional terminal styling support.
 */

#include "core_dynstring.h"
#include "core_format.h"
#include "core_tty.h"

#include "cli_help.h"

#include "app_internal.h"

/** @brief Maximum width for help text formatting */
#define cli_help_max_width 80

/**
 * @brief Generate bold style format argument if styling is enabled
 *
 * Returns a TTY style format argument with bold text formatting
 * if the Style flag is set, otherwise returns a no-op format argument.
 *
 * @param flags Help formatting flags
 * @return Format argument for bold styling or no-op
 */
static FormatArg arg_style_bold(const CliHelpFlags flags) {
    return flags & CliHelpFlags_Style ? fmt_ttystyle(.flags = TtyStyleFlags_Bold) : fmt_nop();
}

/**
 * @brief Generate style reset format argument if styling is enabled
 *
 * Returns a TTY style reset format argument to clear all styling
 * if the Style flag is set, otherwise returns a no-op format argument.
 *
 * @param flags Help formatting flags
 * @return Format argument for style reset or no-op
 */
static FormatArg arg_style_reset(const CliHelpFlags flags) {
    return flags & CliHelpFlags_Style ? fmt_ttystyle() : fmt_nop();
}

/**
 * @brief Check if the application has any options of the specified type
 *
 * Iterates through all registered options to determine if any match
 * the specified option type (flag or argument).
 *
 * @param app The CLI application
 * @param type The option type to search for
 * @return True if options of the specified type exist, false otherwise
 */
static bool cli_help_has_options_of_type(CliApp* app, const CliOptionType type) {
    dynarray_for_t(&app->options, CliOption, opt, {
        if (opt->type == type) {
            return true;
        }
    });

    return false;
}

/**
 * @brief Generate usage string for a single option
 *
 * Creates a formatted usage string showing how the option should be used
 * in command line invocation, including brackets for optional parameters,
 * value placeholders, and multi-value indicators.
 *
 * @param opt The option to generate usage for
 * @return Formatted usage string for the option
 */
static String cli_help_option_usage(CliOption* opt) {
    DynString dynstr = dynstring_create_over(alloc_alloc(g_alloc_scratch, 128, 1));

    const bool optional = (opt->flags & CliOptionFlags_Required) != CliOptionFlags_Required;
    const bool value = (opt->flags & CliOptionFlags_Value) == CliOptionFlags_Value;
    const bool multiValue = (opt->flags & CliOptionFlags_MultiValue) == CliOptionFlags_MultiValue;

    if (optional) {
        fmt_write(&dynstr, "[");
    }

    switch (opt->type) {
        case CliOptionType_Flag: {
            if (opt->dataFlag.character) {
                fmt_write(&dynstr, "-{}", fmt_char(opt->dataFlag.character));
            } else {
                fmt_write(&dynstr, "--{}", fmt_text(opt->dataFlag.name));
            }

            if (value) {
                fmt_write(&dynstr, " <value{}>", multiValue ? fmt_text_lit("...") : fmt_nop());
            }
        } break;

        case CliOptionType_Arg: {
            fmt_write(&dynstr, "<{}{}>", fmt_text(opt->dataFlag.name), multiValue ? fmt_text_lit("...") : fmt_nop());
        } break;
    }

    if (optional) {
        fmt_write(&dynstr, "]");
    }

    String res = dynstring_view(&dynstr);
    dynstring_destroy(&dynstr);

    return res;
}

/**
 * @brief Write the usage line to the help output
 *
 * Generates and formats the usage line showing the application name
 * and all available options with appropriate line wrapping.
 *
 * @param dynstr Target dynamic string for output
 * @param app The CLI application
 * @param flags Help formatting flags
 */
static void cli_help_write_usage(DynString* dynstr, CliApp* app, const CliHelpFlags flags) {
    fmt_write(dynstr, "usage: {}{}{}", arg_style_bold(flags), fmt_text(app->name), arg_style_reset(flags));

    const usize startColumn = string_lit("usage: ").size + app->name.size;
    usize column = startColumn;

    dynarray_for_t(&app->options, CliOption, opt, {
        const String optStr = cli_help_option_usage(opt);
        if ((column + optStr.size + 1) > cli_help_max_width) {
            column = startColumn;
            fmt_write(dynstr, "\n{}", fmt_padding((u16)startColumn));
        }

        fmt_write(dynstr, " {}", fmt_text(optStr));
        column += optStr.size + 1;
    });

    fmt_write(dynstr, "\n");
}

/**
 * @brief Write the arguments section to the help output
 *
 * Generates formatted documentation for all positional arguments
 * including names, required/optional status, and descriptions.
 *
 * @param dynstr Target dynamic string for output
 * @param app The CLI application
 * @param flags Help formatting flags
 */
static void cli_help_write_args(DynString* dynstr, CliApp* app, const CliHelpFlags flags) {
    fmt_write(dynstr, "{}Arguments:{}\n", arg_style_bold(flags), arg_style_reset(flags));

    dynarray_for_t(&app->options, CliOption, opt, {
        if (opt->type != CliOptionType_Arg) {
            continue;
        }

        const bool required = (opt->flags & CliOptionFlags_Required) == CliOptionFlags_Required;

        const String line = fmt_write_scratch(" {<25}{<10}", fmt_text(opt->dataArg.name), required ? fmt_text_lit("REQUIRED") : fmt_text_lit("OPTIONAL"));

        dynstring_append(dynstr, line);

        const String linePrefix = fmt_write_scratch("{}", fmt_padding((u16)line.size));
        format_write_text_wrapped(dynstr, opt->desc, cli_help_max_width - linePrefix.size, linePrefix);

        fmt_write(dynstr, "\n");
    });
}

/**
 * @brief Write the flags section to the help output
 *
 * Generates formatted documentation for all flag options including
 * short and long names, required/optional status, and descriptions.
 *
 * @param dynstr Target dynamic string for output
 * @param app The CLI application
 * @param flags Help formatting flags
 */
static void cli_help_write_flags(DynString* dynstr, CliApp* app, const CliHelpFlags flags) {
    fmt_write(dynstr, "{}Flags:{}\n", arg_style_bold(flags), arg_style_reset(flags));

    dynarray_for_t(&app->options, CliOption, opt, {
        if (opt->type != CliOptionType_Flag) {
            continue;
        }

        const bool required = (opt->flags & CliOptionFlags_Required) == CliOptionFlags_Required;

        const String shortName = opt->dataFlag.character ? fmt_write_scratch("-{},", fmt_char(opt->dataFlag.character)) : string_empty;
        
        const String longName = fmt_write_scratch("--{}", fmt_text(opt->dataFlag.name));

        const String line = fmt_write_scratch(" {<4}{<21}{<10}", fmt_text(shortName), fmt_text(longName), required ? fmt_text_lit("REQUIRED") : fmt_text_lit("OPTIONAL"));

        dynstring_append(dynstr, line);

        const String linePrefix = fmt_write_scratch("{}", fmt_padding((u16)line.size));
        format_write_text_wrapped(dynstr, opt->desc, cli_help_max_width - line.size, linePrefix);

        fmt_write(dynstr, "\n");
    });
}

/**
 * @brief Write complete help documentation to a dynamic string
 *
 * Generates comprehensive help text including usage line, application
 * description, arguments section, and flags section with appropriate
 * formatting and styling.
 *
 * @param dynstr Target dynamic string for output
 * @param app The CLI application
 * @param flags Help formatting flags
 */
void cli_help_write(DynString* dynstr, CliApp* app, const CliHelpFlags flags) {
    cli_help_write_usage(dynstr, app, flags);

    if (!string_is_empty(app->desc)) {
        fmt_write(dynstr, "\n");
        format_write_text_wrapped(dynstr, app->desc, cli_help_max_width, string_empty);
        fmt_write(dynstr, "\n");
    }

    if (cli_help_has_options_of_type(app, CliOptionType_Arg)) {
        fmt_write(dynstr, "\n");
        cli_help_write_args(dynstr, app, flags);
    }

    if (cli_help_has_options_of_type(app, CliOptionType_Flag)) {
        fmt_write(dynstr, "\n");
        cli_help_write_flags(dynstr, app, flags);
    }
}

/**
 * @brief Write complete help documentation to a file
 *
 * Creates a temporary dynamic string, generates comprehensive help text
 * with appropriate styling based on TTY detection, then writes the result
 * to the specified file.
 *
 * @param app The CLI application
 * @param out Output file to write help text to
 */
void cli_help_write_file(CliApp* app, File* out) {
    DynString str = dynstring_create(g_alloc_heap, 1024);

    const CliHelpFlags flags = tty_isatty(out) ? CliHelpFlags_Style : CliHelpFlags_None;
    cli_help_write(&str, app, flags);

    file_write_sync(out, dynstring_view(&str));
    dynstring_destroy(&str);
}