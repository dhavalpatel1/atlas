/**
 * @file parse.c
 * @brief Implementation of CLI argument parsing and validation
 *
 * This file implements the core command-line argument parsing engine including
 * option recognition, value collection, validation, exclusion checking, and
 * error reporting. Provides comprehensive parsing of both short and long flags,
 * positional arguments, and multi-value options with robust error handling.
 */

#include "core_diag.h"
#include "core_dynarray.h"
#include "core_string.h"
#include "core_types.h"

#include "cli_parse.h"

#include "app_internal.h"

/**
 * @brief Internal representation of an option in a parsed invocation
 *
 * Tracks whether an option was provided and stores all values associated
 * with that option during parsing.
 */
typedef struct {
    bool provided;         ///< Whether this option was provided in the invocation
    DynArray values;       ///< Array of String values for this option
} CliInvocationOption;

/**
 * @brief Internal structure representing a parsed CLI invocation
 *
 * Contains all parsing results including errors, option values, and
 * the allocator used for memory management.
 */
struct sCliInvocation {
    Allocator* alloc;      ///< Allocator for memory management
    DynArray errors;       ///< Array of String parsing error messages
    DynArray options;      ///< Array of CliInvocationOption structures
};

/**
 * @brief Internal parsing context for argument processing
 *
 * Contains all state needed during the parsing process including
 * argument iteration, option tracking, and error collection.
 */
typedef struct {
    const CliApp* app;     ///< The CLI application being parsed
    Allocator* alloc;      ///< Allocator for memory management
    bool acceptFlags;      ///< Whether flag parsing is enabled (disabled after --)
    u16 nextPositional;    ///< Index of next expected positional argument
    const char** argHead;  ///< Current position in argument array
    const char** argTail;  ///< End of argument array
    DynArray errors;       ///< Array of String parsing error messages
    DynArray options;      ///< Array of CliInvocationOption structures
} CliParseCtx;

/**
 * @brief Get the invocation option structure for a given option ID
 *
 * @param invoc The CLI invocation
 * @param id The option ID
 * @return Pointer to the CliInvocationOption structure
 */
static CliInvocationOption* cli_invocation_option(CliInvocation* invoc, const CliId id) {
    return dynarray_at_t(&invoc->options, id, CliInvocationOption);
}

/**
 * @brief Add a parsing error to the context
 *
 * Duplicates the error string and adds it to the parsing context's error list.
 *
 * @param ctx The parsing context
 * @param err The error message to add
 */
static void cli_parse_add_error(CliParseCtx* ctx, String err) {
    *dynarray_push_t(&ctx->errors, String) = string_dup(ctx->alloc, err);
}

/**
 * @brief Peek at the current argument without consuming it
 *
 * @param ctx The parsing context
 * @return The current argument string, or empty string if no more arguments
 */
static String cli_parse_peek_arg(CliParseCtx* ctx) {
    return ctx->argHead == ctx->argTail ? string_empty : string_from_null_term(*ctx->argHead);
}

/**
 * @brief Consume the current argument and advance to the next
 *
 * @param ctx The parsing context
 */
static void cli_parse_consume_arg(CliParseCtx* ctx) {
    diag_assert(ctx->argHead != ctx->argTail);

    ++ctx->argHead;
}

/**
 * @brief Get the number of arguments remaining to be parsed
 *
 * @param ctx The parsing context
 * @return Number of remaining arguments
 */
static u32 cli_parse_args_remaining(CliParseCtx* ctx) {
    return ctx->argTail - ctx->argHead;
}

/**
 * @brief Check if an option has already been provided
 *
 * @param ctx The parsing context
 * @param id The option ID to check
 * @return True if the option was already provided, false otherwise
 */
static bool cli_parse_already_provided(CliParseCtx* ctx, CliId id) {
    return dynarray_at_t(&ctx->options, id, CliInvocationOption)->provided;
}

/**
 * @brief Mark an option as provided
 *
 * @param ctx The parsing context
 * @param id The option ID to mark as provided
 */
static void cli_parse_set_provided(CliParseCtx* ctx, CliId id) {
    dynarray_at_t(&ctx->options, id, CliInvocationOption)->provided = true;
}

/**
 * @brief Add a value to an option
 *
 * @param ctx The parsing context
 * @param id The option ID
 * @param value The value to add
 */
static void cli_parse_add_value(CliParseCtx* ctx, const CliId id, const String value) {
    CliInvocationOption* opt = dynarray_at_t(&ctx->options, id, CliInvocationOption);

    *dynarray_push_t(&opt->values, String) = value;
}

/**
 * @brief Parse and add values for an option that accepts values
 *
 * Handles single and multi-value option parsing, consuming arguments
 * until no more values are available or a flag is encountered.
 *
 * @param ctx The parsing context
 * @param optId The option ID to add values for
 */
static void cli_parse_add_values(CliParseCtx* ctx, CliId optId) {
    const CliOptionFlags flags = cli_option(ctx->app, optId)->flags;
    if (!(flags & CliOptionFlags_Value)) {
        return;
    }

    while (cli_parse_args_remaining(ctx) && string_is_empty(cli_parse_peek_arg(ctx))) {
        cli_parse_consume_arg(ctx);
    }

    if (!cli_parse_args_remaining(ctx)) {
        cli_parse_add_error(ctx, fmt_write_scratch("Value missing for option '{}'", fmt_text(cli_option_name(ctx->app, optId))));

        return;
    }

    cli_parse_add_value(ctx, optId, cli_parse_peek_arg(ctx));
    cli_parse_consume_arg(ctx);

    const bool multiValue = (flags & CliOptionFlags_MultiValue) == CliOptionFlags_MultiValue;
    while (multiValue && cli_parse_args_remaining(ctx)) {
        String head = cli_parse_peek_arg(ctx);
        if (string_is_empty(head)) {
            cli_parse_consume_arg(ctx);
            continue;
        }

        if (ctx->acceptFlags && string_starts_with(head, string_lit("-"))) {
            break;
        }

        cli_parse_add_value(ctx, optId, cli_parse_peek_arg(ctx));
        cli_parse_consume_arg(ctx);
    }
}

/**
 * @brief Parse a long flag option (--name)
 *
 * Processes a long flag by name, validates it exists, checks for duplicates,
 * and collects any required values.
 *
 * @param ctx The parsing context
 * @param name The flag name (without -- prefix)
 */
static void cli_parse_long_flag(CliParseCtx* ctx, String name) {
    const CliId optId = cli_find_by_name(ctx->app, name);
    if (sentinel_check(optId)) {
        cli_parse_add_error(ctx, fmt_write_scratch("Unknown flag '{}'", fmt_text(name)));
        return;
    }

    if (cli_parse_already_provided(ctx, optId)) {
        cli_parse_add_error(ctx, fmt_write_scratch("Duplicate flag '{}'", fmt_text(name)));
        return;
    }

    cli_parse_set_provided(ctx, optId);
    cli_parse_add_values(ctx, optId);
}

/**
 * @brief Parse a short flag option (-c)
 *
 * Processes a short flag by character, validates it exists, checks for duplicates,
 * and collects any required values.
 *
 * @param ctx The parsing context
 * @param character The flag character (without - prefix)
 */
static void cli_parse_short_flag(CliParseCtx* ctx, u8 character) {
    const CliId optId = cli_find_by_character(ctx->app, character);
    if (sentinel_check(optId)) {
        cli_parse_add_error(ctx, fmt_write_scratch("Unknown flag '{}'", fmt_char(character)));
        return;
    }

    if (cli_parse_already_provided(ctx, optId)) {
        cli_parse_add_error(ctx, fmt_write_scratch("Duplicate flag '{}'", fmt_char(character)));
        return;
    }

    cli_parse_set_provided(ctx, optId);
    cli_parse_add_values(ctx, optId);
}

/**
 * @brief Parse a block of short flags (-abc)
 *
 * Processes multiple short flags in sequence, validating each character
 * and ensuring none require values (which is not supported in blocks).
 *
 * @param ctx The parsing context
 * @param characterBlock The block of characters (without - prefix)
 */
static void cli_parse_short_flag_block(CliParseCtx* ctx, String characterBlock) {
    mem_for_u8(characterBlock, character, {
        const CliId optId = cli_find_by_character(ctx->app, character);
        if (sentinel_check(optId)) {
            cli_parse_add_error(ctx, fmt_write_scratch("Unknown flag '{}'", fmt_char(character)));
            continue;
        }

        if (cli_parse_already_provided(ctx, optId)) {
            cli_parse_add_error(ctx, fmt_write_scratch("Duplicate flag '{}'", fmt_char(character)));
            continue;
        }

        if (cli_option(ctx->app, optId)->flags & CliOptionFlags_Value) {
            cli_parse_add_error(ctx, fmt_write_scratch("Flag '{}' takes a value", fmt_char(character)));
            continue;
        }

        cli_parse_set_provided(ctx, optId);
    });
}

/**
 * @brief Parse a positional argument
 *
 * Processes the current argument as a positional parameter, finding
 * the appropriate argument option and collecting its value.
 *
 * @param ctx The parsing context
 */
static void cli_parse_arg(CliParseCtx* ctx) {
    const CliId optId = cli_find_by_position(ctx->app, ctx->nextPositional);
    if (sentinel_check(optId)) {
        cli_parse_add_error(ctx, fmt_write_scratch("Invalid input '{}'", fmt_text(cli_parse_peek_arg(ctx))));
        cli_parse_consume_arg(ctx);
        
        return;
    }
    
    ++ctx->nextPositional;

    cli_parse_set_provided(ctx, optId);
    cli_parse_add_values(ctx, optId);
}

/**
 * @brief Main parsing loop for processing all command line arguments
 *
 * Iterates through all arguments, determining their type (flag, argument, separator)
 * and dispatching to appropriate parsing functions. Handles the -- separator
 * to disable flag parsing for remaining arguments.
 *
 * @param ctx The parsing context
 */
static void cli_parse_options(CliParseCtx* ctx) {
    while (cli_parse_args_remaining(ctx)) {
        const String head = cli_parse_peek_arg(ctx);

        if (string_is_empty(head)) {
            cli_parse_consume_arg(ctx);
            continue;
        }

        if (ctx->acceptFlags && string_eq(head, string_lit("--"))) {
            cli_parse_consume_arg(ctx);
            ctx->acceptFlags = false;

            continue;
        }

        if (ctx->acceptFlags && string_eq(head, string_lit("-"))) {
            cli_parse_consume_arg(ctx);
            continue;
        }

        if (ctx->acceptFlags && string_starts_with(head, string_lit("--"))) {
            cli_parse_consume_arg(ctx);
            cli_parse_long_flag(ctx, string_consume(head, 2));

            continue;
        }

        if (ctx->acceptFlags && string_starts_with(head, string_lit("-"))) {
            if (head.size == 2) {
                cli_parse_consume_arg(ctx);
                cli_parse_short_flag(ctx, *string_at(head, 1));
            } else {
                cli_parse_consume_arg(ctx);
                cli_parse_short_flag_block(ctx, string_consume(head, 1));
            }

            continue;
        }

        cli_parse_arg(ctx);
    }
}

/**
 * @brief Run validation for a single option's values
 *
 * Applies the option's validator function to all provided values
 * and reports validation errors.
 *
 * @param ctx The parsing context
 * @param optId The option ID to validate
 */
static void cli_parse_check_validator(CliParseCtx* ctx, const CliId optId) {
    CliInvocationOption* invocOpt = dynarray_at_t(&ctx->options, optId, CliInvocationOption);

    dynarray_for_t(&invocOpt->values, String, val, {
        if (!cli_option(ctx->app, optId)->validator(*val)) {
            const String err = fmt_write_scratch("Invalid input '{}' for option '{}'", fmt_text(*val, .flags = FormatTextFlags_EscapeNonPrintAscii), fmt_text(cli_option_name(ctx->app, optId)));

            cli_parse_add_error(ctx, err);
        }
    });
}

/**
 * @brief Run validation for all options that have validators
 *
 * Iterates through all options and applies their validation functions
 * to the provided values.
 *
 * @param ctx The parsing context
 */
static void cli_parse_check_validators(CliParseCtx* ctx) {
    for (CliId optId = 0; optId != ctx->options.size; ++optId) {
        if (cli_option(ctx->app, optId)->validator) {
            cli_parse_check_validator(ctx, optId);
        }
    }
}

/**
 * @brief Check for violations of mutual exclusion rules
 *
 * Validates that no mutually exclusive options were provided together
 * in the same invocation.
 *
 * @param ctx The parsing context
 */
static void cli_parse_check_exclusions(CliParseCtx* ctx) {
    dynarray_for_t((DynArray*)&ctx->app->exclusions, CliExclusion, ex, {
        if (cli_parse_already_provided(ctx, ex->a) && cli_parse_already_provided(ctx, ex->b)) {
            cli_parse_add_error(ctx, fmt_write_scratch("Options '{}' and '{}' cannot be used together", fmt_text(cli_option_name(ctx->app, ex->a)), fmt_text(cli_option_name(ctx->app, ex->b))));
        }
    });
}

/**
 * @brief Validate that all required options were provided
 *
 * Checks that all options marked as required were provided in the
 * command line invocation.
 *
 * @param ctx The parsing context
 */
static void cli_parse_validate_required_options(CliParseCtx* ctx) {
    for (CliId optId = 0; optId != ctx->options.size; ++optId) {
        CliOption* opt = cli_option(ctx->app, optId);
        const bool isRequired = (opt->flags & CliOptionFlags_Required) == CliOptionFlags_Required;
        if (isRequired && !cli_parse_already_provided(ctx, optId)) {
            cli_parse_add_error(ctx, fmt_write_scratch("Required option '{}' was not provided", fmt_text(cli_option_name(ctx->app, optId))));
        }
    }
}

/**
 * @brief Parse command line arguments according to application specification
 *
 * Main entry point for command line parsing. Creates a parsing context,
 * processes all arguments, performs validation, and returns a complete
 * invocation result with all parsed options and any errors.
 *
 * @param app The CLI application specification
 * @param argc Number of command line arguments
 * @param argv Array of command line argument strings
 * @return Parsed CLI invocation containing results and errors
 */
CliInvocation* cli_parse(const CliApp* app, int argc, const char** argv) {
    diag_assert_msg(argc >= 0, "'argc' (argument count) with a negative value is not supported");

    CliParseCtx ctx = {
        .app = app,
        .alloc = app->alloc,
        .acceptFlags = true,
        .nextPositional = 0,
        .argHead = argv,
        .argTail = argv + argc,
        .errors = dynarray_create_t(app->alloc, String, 0),
        .options = dynarray_create_t(app->alloc, CliInvocationOption, app->options.size),
    };

    for (u32 i = 0; i != app->options.size; ++i) {
        *dynarray_push_t(&ctx.options, CliInvocationOption) = (CliInvocationOption) {
            .provided = false,
            .values = dynarray_create_t(app->alloc, String, 0)
        };
    }

    cli_parse_options(&ctx);
    cli_parse_check_validators(&ctx);
    cli_parse_check_exclusions(&ctx);
    cli_parse_validate_required_options(&ctx);

    CliInvocation* invoc = alloc_alloc_t(app->alloc, CliInvocation);
    *invoc = (CliInvocation) {
        .alloc = app->alloc,
        .errors = ctx.errors,
        .options = ctx.options,
    };

    return invoc;
}

/**
 * @brief Destroy a CLI invocation and free all associated resources
 *
 * Frees all error messages, option values, and the invocation structure itself.
 *
 * @param invoc The CLI invocation to destroy
 */
void cli_parse_destroy(CliInvocation* invoc) {
    dynarray_for_t(&invoc->errors, String, err, { string_free(invoc->alloc, *err); });
    dynarray_destroy(&invoc->errors);

    dynarray_for_t(&invoc->options, CliInvocationOption, opt, { dynarray_destroy(&opt->values); });
    dynarray_destroy(&invoc->options);

    alloc_free_t(invoc->alloc, invoc);
}

/**
 * @brief Get the overall parsing result status
 *
 * @param invoc The CLI invocation
 * @return Success if no errors occurred, Fail if there were parsing errors
 */
CliParseResult cli_parse_result(CliInvocation* invoc) {
    return invoc->errors.size ? CliParseResult_Fail : CliParseResult_Success;
}

/**
 * @brief Get all parsing errors from the invocation
 *
 * @param invoc The CLI invocation
 * @return Structure containing pointer to error array and count
 */
CliParseErrors cli_parse_errors(CliInvocation* invoc) {
    return (CliParseErrors) {
        .head = invoc->errors.size ? dynarray_at_t(&invoc->errors, 0, String) : null,
        .count = invoc->errors.size,
    };
}

/**
 * @brief Check if an option was provided in the invocation
 *
 * @param invoc The CLI invocation
 * @param id The option ID to check
 * @return True if the option was provided, false otherwise
 */
bool cli_parse_provided(CliInvocation* invoc, CliId id) {
    return cli_invocation_option(invoc, id)->provided;
}

/**
 * @brief Get all values provided for an option
 *
 * @param invoc The CLI invocation
 * @param id The option ID
 * @return Structure containing pointer to value array and count
 */
CliParseValues cli_parse_values(CliInvocation* invoc, CliId id) {
    const CliInvocationOption* opt = cli_invocation_option(invoc, id);

    return (CliParseValues) {
        .head = opt->values.size ? dynarray_at_t(&opt->values, 0, String) : null,
        .count = opt->values.size
    };
}