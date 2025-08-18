/**
 * @file lex.h
 * @brief Internal JSON lexical analyzer (tokenizer) interface
 *
 * This header defines the internal tokenizer used by the JSON parser to convert
 * raw text into structured tokens. The lexer handles all JSON token types including
 * literals, strings with escape sequences, and error conditions.
 */

#pragma once

#include "core_string.h"
#include "json_parse.h"

/**
 * @brief JSON token types enumeration
 */
typedef enum {
    JsonTokenType_BracketOpen,      /**< '[' - Array start */
    JsonTokenType_BracketClose,     /**< ']' - Array end */
    JsonTokenType_CurlyOpen,        /**< '{' - Object start */
    JsonTokenType_CurlyClose,       /**< '}' - Object end */
    JsonTokenType_Comma,            /**< ',' - Value separator */
    JsonTokenType_Colon,            /**< ':' - Name/value separator */
    JsonTokenType_String,           /**< String literal */
    JsonTokenType_Number,           /**< Numeric literal */
    JsonTokenType_True,             /**< 'true' boolean literal */
    JsonTokenType_False,            /**< 'false' boolean literal */
    JsonTokenType_Null,             /**< 'null' literal */
    JsonTokenType_Error,            /**< Lexical error occurred */
    JsonTokenType_End,              /**< End of input reached */
} JsonTokenType;

/**
 * @brief JSON token structure containing type and value data
 */
typedef struct {
    JsonTokenType type;         /**< Type of the token */
    union {
        String val_string;      /**< String value (for string tokens) */
        f64 val_number;         /**< Numeric value (for number tokens) */
        JsonError val_error;    /**< Error code (for error tokens) */
    };
} JsonToken;

/**
 * @brief Tokenize the next JSON token from input string
 * @param input Input string to tokenize
 * @param token Pointer to store the extracted token
 * @return Remaining portion of input string after the token
 *
 * Extracts the next JSON token from the input string, handling:
 * - Whitespace skipping
 * - String parsing with escape sequences (including Unicode)
 * - Number parsing (integers and floating-point)
 * - Boolean and null literals
 * - Structural characters (brackets, braces, comma, colon)
 * - Error detection and reporting
 */
String json_lex(String input, JsonToken* token);