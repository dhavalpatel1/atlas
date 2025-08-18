/**
 * @file json_parse.h
 * @brief JSON parsing functionality for converting text to JSON documents
 *
 * This header provides JSON parsing capabilities that convert JSON text strings
 * into JSON document structures. The parser is RFC 7159 compliant and provides
 * detailed error reporting for invalid JSON input.
 */

#pragma once

#include "json_doc.h"

/**
 * @brief JSON parsing result type enumeration
 */
typedef enum {
    JsonResultType_Success,     /**< Parsing completed successfully */
    JsonResultType_Fail,        /**< Parsing failed with an error */
} JsonResultType;

/**
 * @brief JSON parsing error enumeration
 */
typedef enum {
    JsonError_DuplicateField,           /**< Object contains duplicate field names */
    JsonError_InvalidChar,              /**< Invalid character encountered */
    JsonError_InvalidCharInFalse,       /**< Invalid character in 'false' literal */
    JsonError_InvalidCharInNull,        /**< Invalid character in 'null' literal */
    JsonError_InvalidCharInString,      /**< Invalid character in string value */
    JsonError_InvalidCharInTrue,        /**< Invalid character in 'true' literal */
    JsonError_InvalidEscapeSequence,    /**< Invalid escape sequence in string */
    JsonError_InvalidFieldName,         /**< Invalid field name in object */
    JsonError_InvalidFieldSeperator,    /**< Invalid field separator in object */
    JsonError_MaximumDepthExceeded,     /**< Maximum nesting depth exceeded */
    JsonError_TooLongString,            /**< String value exceeds maximum length */
    JsonError_Truncated,                /**< Input was truncated unexpectedly */
    JsonError_UnexpectedToken,          /**< Unexpected token encountered */
    JsonError_UnterminatedString,       /**< String value was not properly terminated */

    JsonError_Count,                    /**< Number of error types */
} JsonError;

/**
 * @brief JSON parsing result structure
 */
typedef struct {
    JsonResultType type;        /**< Result type (success or failure) */
    union {
        JsonVal val;            /**< Parsed JSON value (when successful) */
        JsonError error;        /**< Error code (when failed) */
    };
} JsonResult;

/**
 * @brief Get a human-readable string for a JSON parsing error
 * @param error JSON error code
 * @return String description of the error
 */
String json_error_str(JsonError error);

/**
 * @brief Parse JSON text and add the result to a JSON document
 * @param doc JSON document to add the parsed value to
 * @param input JSON text string to parse
 * @param res Pointer to store the parsing result
 * @return Remaining unparsed portion of the input string
 *
 * The function parses a single JSON value from the input string and adds it to
 * the document. If parsing succeeds, res->val contains the handle to the parsed
 * value. If parsing fails, res->error contains the error code.
 */
String json_read(JsonDoc* doc, String input, JsonResult* res);