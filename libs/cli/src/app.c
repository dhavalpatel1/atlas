/**
 * @file app.c
 * @brief Implementation of CLI application management and option registration
 *
 * This file implements the core functionality for creating, configuring, and managing
 * CLI applications including option registration (flags and arguments), validation
 * setup, exclusion rules, and resource cleanup. Provides the foundational API for
 * building command-line interfaces with the CLI library.
 */

#include "core_ascii.h"
#include "core_diag.h"
#include "core_path.h"

#include "app_internal.h"

/** @brief Maximum length allowed for option names */
#define cli_app_option_name_max_len 64

/**
 * @brief Check if two options have an exclusion rule between them
 *
 * Searches through the application's exclusion rules to determine if
 * the specified options are mutually exclusive.
 *
 * @param app The CLI application
 * @param a First option ID
 * @param b Second option ID
 * @return True if the options exclude each other, false otherwise
 */
MAYBE_UNUSED static bool cli_app_excludes(CliApp* app, const CliId a, const CliId b) {
    dynarray_for_t(&app->exclusions, CliExclusion, ex, {
        if (ex->a == a && ex->b == b) {
            return true;
        }

        if (ex->b == a && ex->a == b) {
            return true;
        }
    });

    return false;
}

/**
 * @brief Create a new CLI application
 *
 * Allocates and initializes a new CliApp structure with the provided description.
 * The application name is automatically derived from the executable path.
 * Initializes empty option and exclusion arrays.
 *
 * @param alloc The allocator to use for memory management
 * @param desc Application description (can be empty)
 * @return Pointer to the newly created CliApp
 */
CliApp* cli_app_create(Allocator* alloc, String desc) {
    CliApp* app = alloc_alloc_t(alloc, CliApp);
    *app = (CliApp) {
        .name = path_stem(g_path_executable),
        .desc = string_is_empty(desc) ? string_empty : string_dup(alloc, desc),
        .options = dynarray_create_t(alloc, CliOption, 16),
        .exclusions = dynarray_create_t(alloc, CliExclusion, 8),
        .alloc = alloc
    };

    return app;
}

/**
 * @brief Destroy a CLI application and free all associated resources
 *
 * Cleans up all registered options, frees their names and descriptions,
 * destroys the option and exclusion arrays, and frees the CliApp structure.
 *
 * @param app The CLI application to destroy
 */
void cli_app_destroy(CliApp* app) {
    if (!string_is_empty(app->desc)) {
        string_free(app->alloc, app->desc);
    }

    dynarray_for_t(&app->options, CliOption, opt, {
        if (opt->desc.ptr) {
            string_free(app->alloc, opt->desc);
        }

        switch (opt->type) {
            case CliOptionType_Flag: {
                string_free(app->alloc, opt->dataFlag.name);
            } break;

            case CliOptionType_Arg: {
                string_free(app->alloc, opt->dataArg.name);
            } break;
        }
    });

    dynarray_destroy(&app->options);
    dynarray_destroy(&app->exclusions);

    alloc_free_t(app->alloc, app);
}

/**
 * @brief Register a flag option with the CLI application
 *
 * Adds a new flag option that can be accessed via short character (e.g., -h)
 * or long name (e.g., --help). Validates that the character is printable ASCII
 * and that no duplicate options exist.
 *
 * @param app The CLI application
 * @param character Short character for the flag (0 for none)
 * @param name Long name for the flag
 * @param flags Option configuration flags
 * @return Unique ID for the registered flag option
 */
CliId cli_register_flag(CliApp* app, u8 character, String name, CliOptionFlags flags) {
    diag_assert_msg(!string_is_empty(name), "Flag needs a name");
    diag_assert_msg(name.size <= cli_app_option_name_max_len, "Flag name too long");

    diag_assert_msg(character == '\0' || ascii_is_printable(character), "Character '{}' is not printable ascii", fmt_char(character, .flags = FormatTextFlags_EscapeNonPrintAscii));

    diag_assert_msg(character == '\0' || sentinel_check(cli_find_by_character(app, character)), "Duplicate flag with character '{}'", fmt_char(character));

    diag_assert_msg(sentinel_check(cli_find_by_name(app, name)), "Duplicate flag with name '{}'", fmt_text(name));

    const CliId id = (CliId)app->options.size;

    *dynarray_push_t(&app->options, CliOption) = (CliOption) {
        .type = CliOptionType_Flag,
        .flags = flags,
        .desc = string_empty,
        .dataFlag = {
            .character = character,
            .name = string_dup(app->alloc, name),
        }
    };

    return id;
}

/**
 * @brief Register a positional argument with the CLI application
 *
 * Adds a new positional argument option. Arguments are automatically assigned
 * positions based on registration order and always have the Value flag set.
 *
 * @param app The CLI application
 * @param name Name for the argument
 * @param flags Option configuration flags (Value flag is automatically added)
 * @return Unique ID for the registered argument option
 */
CliId cli_register_arg(CliApp* app, const String name, CliOptionFlags flags) {
    diag_assert_msg(!string_is_empty(name), "Argument needs a name");
    diag_assert_msg(name.size <= cli_app_option_name_max_len, "Argument name too long");

    u16 position = 0;
    dynarray_for_t(&app->options, CliOption, opt, {
        if (opt->type == CliOptionType_Arg) {
            ++position;
        }
    });

    const CliId id = (CliId)app->options.size;

    *dynarray_push_t(&app->options, CliOption) = (CliOption) {
        .type = CliOptionType_Arg,
        .flags = flags | CliOptionFlags_Value,
        .desc = string_empty,
        .dataArg = {
            .position = position,
            .name = string_dup(app->alloc, name)
        }
    };

    return id;
}

/**
 * @brief Register a validation function for an option
 *
 * Associates a validation function with the specified option. The validator
 * will be called during parsing to validate option values. Only options that
 * accept values can have validators.
 *
 * @param app The CLI application
 * @param id The option ID to add validation to
 * @param validator The validation function
 */
void cli_register_validator(CliApp* app, CliId id, CliValidateFunc validator) {
    CliOption* opt = cli_option(app, id);

    diag_assert_msg(!opt->validator, "Option '{}' already has a validator registered", fmt_text(cli_option_name(app, id)));

    diag_assert_msg(opt->flags & CliOptionFlags_Value, "Option '{}' doesn't take a value and thus cannot register a validator", fmt_text(cli_option_name(app, id)));

    opt->validator = validator;
}

/**
 * @brief Register a mutual exclusion rule between two options
 *
 * Establishes that two options cannot be used together in the same invocation.
 * The exclusion is bidirectional (a excludes b, and b excludes a).
 *
 * @param app The CLI application
 * @param a First option ID
 * @param b Second option ID
 */
void cli_register_exclusion(CliApp* app, const CliId a, const CliId b) {
    diag_assert_msg(!cli_app_excludes(app, a, b), "There is already a exclusion between '{}' and '{}'", fmt_text(cli_option_name(app, a)), fmt_text(cli_option_name(app, b)));

    *dynarray_push_t(&app->exclusions, CliExclusion) = (CliExclusion){ a, b };
}

/**
 * @brief Register a description for an option
 *
 * Associates a human-readable description with the specified option.
 * The description is used in help output generation.
 *
 * @param app The CLI application
 * @param id The option ID
 * @param desc The description text (will be duplicated)
 */
void cli_register_desc(CliApp* app, const CliId id, String desc) {
    diag_assert_msg(!string_is_empty(desc), "Empty descriptions are not supported");

    CliOption* opt = cli_option(app, id);

    diag_assert_msg(string_is_empty(opt->desc), "Option '{}' already has a description registered", fmt_text(cli_option_name(app, id)));

    if (opt->desc.ptr) {
        string_free(app->alloc, opt->desc);
    }

    opt->desc = string_dup(app->alloc, desc);
}

/**
 * @brief Get a pointer to an option by its ID
 *
 * Retrieves the option structure for the specified ID with bounds checking.
 *
 * @param app The CLI application
 * @param id The option ID
 * @return Pointer to the CliOption structure
 */
CliOption* cli_option(const CliApp* app, CliId id) {
    diag_assert_msg(id < app->options.size, "Out of bounds CliId");

    return dynarray_at_t(&app->options, id, CliOption);
}

/**
 * @brief Get the name of an option by its ID
 *
 * Returns the name string for the specified option, handling both
 * flag and argument option types.
 *
 * @param app The CLI application
 * @param id The option ID
 * @return The option's name string
 */
String cli_option_name(const CliApp* app, CliId id) {
    const CliOption* opt = cli_option(app, id);

    switch (opt->type) {
        case CliOptionType_Flag: {
            return opt->dataFlag.name;
        }

        case CliOptionType_Arg: {
            return opt->dataArg.name;
        }
    };

    diag_crash_msg("Unsupported option type");
}

/**
 * @brief Find a flag option by its short character
 *
 * Searches for a flag option that matches the specified character.
 *
 * @param app The CLI application
 * @param character The character to search for
 * @return Option ID if found, sentinel_u16 if not found
 */
CliId cli_find_by_character(const CliApp* app, u8 character) {
    diag_assert_msg(character, "Null is not a valid flag character");

    dynarray_for_t((DynArray*)&app->options, CliOption, opt, {
        if (opt->type == CliOptionType_Flag && opt->dataFlag.character == character) {
            return (CliId)opt_i;
        }
    });

    return sentinel_u16;
}

/**
 * @brief Find a flag option by its long name
 *
 * Searches for a flag option that matches the specified name string.
 *
 * @param app The CLI application
 * @param name The name to search for
 * @return Option ID if found, sentinel_u16 if not found
 */
CliId cli_find_by_name(const CliApp* app, String name) {
    diag_assert_msg(!string_is_empty(name), "Empty string is not a valid flag name");

    dynarray_for_t((DynArray*)&app->options, CliOption, opt, {
        if (opt->type == CliOptionType_Flag && string_eq(opt->dataFlag.name, name)) {
            return (CliId)opt_i;
        }
    });

    return sentinel_u16;
}

/**
 * @brief Find an argument option by its position
 *
 * Searches for an argument option at the specified positional index.
 *
 * @param app The CLI application
 * @param position The position to search for
 * @return Option ID if found, sentinel_u16 if not found
 */
CliId cli_find_by_position(const CliApp* app, u16 position) {
    dynarray_for_t((DynArray*)&app->options, CliOption, opt, {
        if (opt->type == CliOptionType_Arg && opt->dataArg.position == position) {
            return (CliId)opt_i;
        }
    });

    return sentinel_u16;
}