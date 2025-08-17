/**
 * @file cli_app.h
 * @brief CLI application definition and configuration functions
 *
 * This header provides the core structures and functions for creating and
 * configuring CLI applications. It includes support for registering command-line
 * flags, arguments, validators, and descriptions. The CliApp structure represents
 * a complete command-line interface specification.
 */

#pragma once

#include "core_alloc.h"
#include "core_string.h"

/**
 * @brief Function pointer type for validating CLI input values
 *
 * @param input The input string to validate
 * @return true if the input is valid, false otherwise
 */
typedef bool (*CliValidateFunc)(const String input);

/**
 * @brief Unique identifier type for CLI options and arguments
 *
 * Used to reference registered flags and arguments within the CLI system.
 * IDs are assigned automatically when registering options.
 */
typedef u16 CliId;

/**
 * @brief Flags controlling the behavior of CLI options and arguments
 */
typedef enum {
    CliOptionFlags_None = 0,                                        /**< No special flags */
    CliOptionFlags_Value = 1 << 0,                                  /**< Option accepts a value */
    CliOptionFlags_MultiValue = (1 << 1) | CliOptionFlags_Value,    /**< Option accepts multiple values */
    CliOptionFlags_Required = (1 << 2) | CliOptionFlags_Value,      /**< Option is required and accepts a value */
} CliOptionFlags;

/**
 * @brief Opaque structure representing a CLI application configuration
 *
 * This structure holds the complete specification for a command-line interface,
 * including registered flags, arguments, validators, and help descriptions.
 * It should only be accessed through the provided API functions.
 */
typedef struct sCliApp CliApp;

/**
 * @brief Create a new CLI application
 *
 * @param allocator The allocator to use for memory management
 * @param desc The description of the application for help text
 * @return Pointer to the newly created CliApp, or null on failure
 */
CliApp* cli_app_create(Allocator*, String desc);

/**
 * @brief Destroy a CLI application and free its resources
 *
 * @param app The CliApp to destroy
 */
void cli_app_destroy(CliApp*);

/**
 * @brief Register a command-line flag option
 *
 * @param app The CLI application to register the flag with
 * @param character The single character shorthand for the flag (e.g., 'v' for -v)
 * @param name The long name for the flag (e.g., "verbose" for --verbose)
 * @param flags Flags controlling the behavior of this option
 * @return The unique ID assigned to this flag
 */
CliId cli_register_flag(CliApp*, u8 character, String name, CliOptionFlags);

/**
 * @brief Register a positional command-line argument
 *
 * @param app The CLI application to register the argument with
 * @param name The name of the argument for help text and identification
 * @param flags Flags controlling the behavior of this argument
 * @return The unique ID assigned to this argument
 */
CliId cli_register_arg(CliApp*, String name, CliOptionFlags);

/**
 * @brief Register a validation function for a CLI option or argument
 *
 * @param app The CLI application
 * @param id The ID of the option or argument to validate
 * @param validator The validation function to apply to input values
 */
void cli_register_validator(CliApp*, CliId, CliValidateFunc);

/**
 * @brief Register mutual exclusion between two CLI options
 *
 * Ensures that the two specified options cannot be used together.
 *
 * @param app The CLI application
 * @param a The ID of the first mutually exclusive option
 * @param b The ID of the second mutually exclusive option
 */
void cli_register_exclusion(CliApp*, CliId a, CliId b);

/**
 * @brief Register a description for a CLI option or argument
 *
 * @param app The CLI application
 * @param id The ID of the option or argument to describe
 * @param desc The description text to display in help output
 */
void cli_register_desc(CliApp*, CliId, String desc);