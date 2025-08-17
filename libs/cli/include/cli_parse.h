/**
 * @file cli_parse.h
 * @brief CLI command-line parsing functions and result structures
 *
 * This header provides the core functionality for parsing command-line arguments
 * against a CLI application specification. It includes structures for handling
 * parse results, errors, and extracted values, as well as functions for querying
 * the parsing outcome.
 */

#pragma once

#include "core_string.h"
#include "core_types.h"

/**
 * @brief Forward declaration of CliApp structure
 */
typedef struct sCliApp CliApp;

/**
 * @brief Unique identifier type for CLI options and arguments
 */
typedef u16 CliId;

/**
 * @brief Result codes for CLI parsing operations
 */
typedef enum {
    CliParseResult_Success = 0,  /**< Parsing completed successfully */
    CliParseResult_Fail = 1,     /**< Parsing failed with errors */
} CliParseResult;

/**
 * @brief Container for parsing error messages
 *
 * Holds an array of error strings generated during command-line parsing.
 */
typedef struct {
    String* head;   /**< Pointer to the first error string in the array */
    usize count;    /**< Number of error strings in the array */
} CliParseErrors;

/**
 * @brief Container for parsed argument values
 *
 * Holds an array of string values associated with a specific CLI option or argument.
 */
typedef struct {
    String* head;   /**< Pointer to the first value string in the array */
    usize count;    /**< Number of value strings in the array */
} CliParseValues;

/**
 * @brief Opaque structure representing the result of CLI parsing
 *
 * This structure contains the complete result of parsing command-line arguments,
 * including any errors, extracted values, and metadata. It should only be
 * accessed through the provided API functions.
 */
typedef struct sCliInvocation CliInvocation;

/**
 * @brief Parse command-line arguments against a CLI application specification
 *
 * @param app The CLI application specification to parse against
 * @param argc The number of command-line arguments
 * @param argv Array of command-line argument strings
 * @return Pointer to a CliInvocation containing the parse results, or null on allocation failure
 */
CliInvocation* cli_parse(const CliApp*, int argc, const char** argv);

/**
 * @brief Destroy a CLI invocation and free its resources
 *
 * @param invocation The CliInvocation to destroy
 */
void cli_parse_destroy(CliInvocation*);

/**
 * @brief Get the overall result of the parsing operation
 *
 * @param invocation The CLI invocation to query
 * @return The parse result code (success or failure)
 */
CliParseResult cli_parse_result(CliInvocation*);

/**
 * @brief Get the parsing errors from a CLI invocation
 *
 * @param invocation The CLI invocation to query
 * @return Structure containing any error messages generated during parsing
 */
CliParseErrors cli_parse_errors(CliInvocation*);

/**
 * @brief Check if a specific option or argument was provided
 *
 * @param invocation The CLI invocation to query
 * @param id The ID of the option or argument to check
 * @return true if the option was provided, false otherwise
 */
bool cli_parse_provided(CliInvocation*, CliId);

/**
 * @brief Get the values associated with a specific option or argument
 *
 * @param invocation The CLI invocation to query
 * @param id The ID of the option or argument
 * @return Structure containing the values for the specified option
 */
CliParseValues cli_parse_values(CliInvocation*, CliId);