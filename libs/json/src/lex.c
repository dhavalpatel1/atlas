/**
 * @file lex.c
 * @brief JSON lexical analyzer implementation
 *
 * Implements the JSON tokenizer that breaks JSON text into tokens for parsing.
 * Handles all JSON syntax including string escape sequences, Unicode support,
 * numeric literals, and proper error reporting for malformed input.
 */

#include "core_ascii.h"
#include "core_diag.h"
#include "core_format.h"
#include "core_utf8.h"

#include "lex.h"

/** @brief Maximum allowed size for JSON string values (64 KiB) */
#define json_string_max_size (usize_kibibyte * 64)

/**
 * @brief Create an error token with the specified error code
 * @param _ERR_ Error code to include in the token
 * @return JsonToken structure configured as an error token
 */
#define json_token_err(_ERR_)        \
    (JsonToken) {                    \
        .type = JsonTokenType_Error, \
        .val_error = (_ERR_)         \
    }

/**
 * @brief Parse a numeric literal from JSON input
 * @param str Input string starting with a number
 * @param out Token structure to store the parsed number
 * @return Remaining input after the number
 *
 * Parses JSON numbers including integers, decimals, and scientific notation.
 * Uses the core format reader for IEEE 754 double precision parsing.
 */
static String json_lex_number(String str, JsonToken* out) {
    out->type = JsonTokenType_Number;

    return format_read_f64(str, &out->val_number);
}

/**
 * @brief Parse a string literal from JSON input with escape sequence handling
 * @param str Input string starting with opening quote
 * @param out Token structure to store the parsed string
 * @return Remaining input after the closing quote
 *
 * Handles all JSON escape sequences including:
 * - Standard escapes: \", \\, \/, \b, \f, \n, \r, \t
 * - Unicode escapes: \uXXXX (4 hex digits)
 * - Error detection for invalid sequences and unterminated strings
 */
static String json_lex_string(String str, JsonToken* out) {
    diag_assert(*string_begin(str) == '"');
    str = string_consume(str, 1);

    DynString result = dynstring_create_over(alloc_alloc(g_alloc_scratch, json_string_max_size, 1));

    bool escaped = false;
    while (true) {
        if (UNLIKELY(result.size >= json_string_max_size)) {
            *out = json_token_err(JsonError_TooLongString);
            goto Ret;
        } 

        if (UNLIKELY(string_is_empty(str))) {
            *out = json_token_err(JsonError_UnterminatedString);
            goto Ret;
        }

        const u8 ch = *string_begin(str);
        str = string_consume(str, 1);

        if (escaped) {
            switch (ch) {
                case '"': {
                    dynstring_append_char(&result, '"');
                } break;

                case '\\': {
                    dynstring_append_char(&result, '\\');
                } break;

                case '/': {
                    dynstring_append_char(&result, '/');
                } break;

                case 'b': {
                    dynstring_append_char(&result, '\b');
                } break;

                case 'f': {
                    dynstring_append_char(&result, '\f');
                } break;

                case 'n': {
                    dynstring_append_char(&result, '\n');
                } break;

                case 'r': {
                    dynstring_append_char(&result, '\r');
                } break;

                case 't': {
                    dynstring_append_char(&result, '\t');
                } break;

                case 'u':
                case 'U': {
                    u64 unicodePoint;
                    str = format_read_u64(str, &unicodePoint, 16);
                    utf8_cp_write(&result, (Utf8Codepoint)unicodePoint);
                } break;

                default: {
                    *out = json_token_err(JsonError_InvalidEscapeSequence);
                    goto Ret;
                }
            }

            escaped = false;
            continue;
        }

        switch (ch) {
            case '\\': {
                escaped = true;
            } break;

            case '"': {
                out->type = JsonTokenType_String;
                out->val_string = dynstring_view(&result);
                
                goto Ret;
            }

            default: {
                if (UNLIKELY(ascii_is_control(ch))) {
                    *out = json_token_err(JsonError_InvalidCharInString);

                    goto Ret;
                }

                dynstring_append_char(&result, ch);
            }
        }
    }

Ret:
    dynstring_destroy(&result);
    
    return str;
}

/**
 * @brief Parse the 'true' boolean literal
 * @param str Input string starting with 't'
 * @param out Token structure to store the result
 * @return Remaining input after 'true' or after error character
 *
 * Validates that the input contains the complete 'true' literal.
 * If validation fails, consumes one character and reports an error.
 */
static String json_lex_true(String str, JsonToken* out) {
    if (LIKELY(string_starts_with(str, string_lit("true")))) {
        out->type = JsonTokenType_True;

        return string_consume(str, 4);
    }

    *out = json_token_err(JsonError_InvalidCharInTrue);
    
    return string_consume(str, 1);
}

/**
 * @brief Parse the 'false' boolean literal
 * @param str Input string starting with 'f'
 * @param out Token structure to store the result
 * @return Remaining input after 'false' or after error character
 *
 * Validates that the input contains the complete 'false' literal.
 * If validation fails, consumes one character and reports an error.
 */
static String json_lex_false(String str, JsonToken* out) {
    if (LIKELY(string_starts_with(str, string_lit("false")))) {
        out->type = JsonTokenType_False;

        return string_consume(str, 5);
    }

    *out = json_token_err(JsonError_InvalidCharInFalse);
    
    return string_consume(str, 1);
}

/**
 * @brief Parse the 'null' literal
 * @param str Input string starting with 'n'
 * @param out Token structure to store the result
 * @return Remaining input after 'null' or after error character
 *
 * Validates that the input contains the complete 'null' literal.
 * If validation fails, consumes one character and reports an error.
 */
static String json_lex_null(String str, JsonToken* out) {
    if (LIKELY(string_starts_with(str, string_lit("null")))) {
        out->type = JsonTokenType_Null;

        return string_consume(str, 4);
    }

    *out = json_token_err(JsonError_InvalidCharInNull);
    
    return string_consume(str, 1);
}

String json_lex(String str, JsonToken* out) {
    while (!string_is_empty(str)) {
        switch (*string_begin(str)) {
            case '[': {
                out->type = JsonTokenType_BracketOpen;

                return string_consume(str, 1);
            }

            case ']': {
                out->type = JsonTokenType_BracketClose;

                return string_consume(str, 1);
            }

            case '{': {
                out->type = JsonTokenType_CurlyOpen;

                return string_consume(str, 1);
            }

            case '}': {
                out->type = JsonTokenType_CurlyClose;

                return string_consume(str, 1);
            }

            case ',': {
                out->type = JsonTokenType_Comma;

                return string_consume(str, 1);
            }

            case ':': {
                out->type = JsonTokenType_Colon;

                return string_consume(str, 1);
            }

            case '"': {
                return json_lex_string(str, out);
            }

            case 't': {
                return json_lex_true(str, out);
            }

            case 'f': {
                return json_lex_false(str, out);
            }

            case 'n': {
                return json_lex_null(str, out);
            }

            case '-':
            case '.':
            case '0':
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9': {
                return json_lex_number(str, out);
            }

            case ' ':
            case '\n':
            case '\r':
            case '\t': {
                str = string_consume(str, 1);
                continue;
            }
            
            default: {
                *out = json_token_err(JsonError_InvalidChar);

                return string_consume(str, 1);
            }
        }
    }

    out->type = JsonTokenType_End;

    return string_empty;
}