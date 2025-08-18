/**
 * @file parse.c
 * @brief JSON parsing implementation
 *
 * Implements recursive descent JSON parser that converts tokenized input into
 * JSON document structures. Handles nested objects and arrays with depth limiting
 * and comprehensive error reporting.
 */

#include "core_array.h"
#include "core_diag.h"

#include "json_parse.h"

#include "lex.h"

/** @brief Maximum allowed nesting depth for JSON structures */
#define json_depth_max 100

/**
 * @brief Create a failure result with the specified error
 * @param _ERR_ Error code to include in the result
 * @return JsonResult structure configured as a failure
 */
#define json_err(_ERR_)              \
    (JsonResult) {                   \
        .type = JsonResultType_Fail, \
        .error = (_ERR_)             \
    }

/**
 * @brief Create a success result with the specified JSON value
 * @param _VAL_ JSON value handle to include in the result
 * @return JsonResult structure configured as a success
 */
#define json_success(_VAL_)             \
    (JsonResult) {                      \
        .type = JsonResultType_Success, \
        .val = (_VAL_)                  \
    }

/** @brief String representations of JSON parsing errors */
static const String g_error_strs[] = {
    string_static("DuplicateField"),
    string_static("InvalidChar"),
    string_static("InvalidCharInFalse"),
    string_static("InvalidCharInNull"),
    string_static("InvalidCharInString"),
    string_static("InvalidCharInTrue"),
    string_static("InvalidEscapeSequence"),
    string_static("InvalidFieldName"),
    string_static("InvalidFieldSeperator"),
    string_static("MaximumDepthExceeded"),
    string_static("TooLongString"),
    string_static("Truncated"),
    string_static("UnexpectedToken"),
    string_static("UnterminatedString"),
};

_Static_assert(array_elems(g_error_strs) == JsonError_Count, "Incorrect number of JsonError strings");

String json_error_str(JsonError error) {
    diag_assert(error < JsonError_Count);

    return g_error_strs[error];
}

/**
 * @brief Parse a JSON value given its starting token
 * @param doc JSON document to add parsed value to
 * @param input Remaining input string after the start token
 * @param startToken First token of the value to parse
 * @param res Pointer to store parsing result
 * @return Remaining input after parsing the value
 */
static String json_read_with_start_token(JsonDoc* doc, String input, JsonToken startToken, JsonResult* res);

/**
 * @brief Parse a JSON array from input
 * @param doc JSON document to add parsed array to
 * @param input Input string positioned after opening bracket
 * @param res Pointer to store parsing result
 * @return Remaining input after parsing the array
 *
 * Parses array elements recursively and handles trailing commas (non-standard extension).
 */
static String json_read_array(JsonDoc* doc, String input, JsonResult* res) {
    const JsonVal array = json_add_array(doc);

    JsonToken token;
    JsonResult valRes;

    while (true) {
        input = json_lex(input, &token);
        if (token.type == JsonTokenType_BracketClose) {
            // NOTE: Not according to spec but we accept trailing comma's
            goto Success;
        }

        input = json_read_with_start_token(doc, input, token, &valRes);
        if (valRes.type == JsonResultType_Fail) {
            *res = json_err(valRes.error);

            return input;
        }
        json_add_elem(doc, array, valRes.val);

        input = json_lex(input, &token);
        switch (token.type) {
            case JsonTokenType_BracketClose: {
                goto Success;
            }

            case JsonTokenType_Comma: {
                
            } break;

            case JsonTokenType_End: {
                *res = json_err(JsonError_Truncated);
            } break;

            case JsonTokenType_Error: {
                *res = json_err(token.val_error);
            } break;

            default: {
                *res = json_err(JsonError_UnexpectedToken);
                
                return input;
            }
        }
    }

Success:
    *res = json_success(array);

    return input;
}

/**
 * @brief Parse a JSON object from input
 * @param doc JSON document to add parsed object to
 * @param input Input string positioned after opening brace
 * @param res Pointer to store parsing result
 * @return Remaining input after parsing the object
 *
 * Parses object fields recursively, validates field name uniqueness,
 * and handles trailing commas (non-standard extension).
 */
static String json_read_object(JsonDoc* doc, String input, JsonResult* res) {
    const JsonVal object = json_add_object(doc);

    JsonToken token;
    JsonResult valRes;
    while (true) {
        input = json_lex(input, &token);
        if (token.type == JsonTokenType_CurlyClose) {
            goto Success;
        }

        if (token.type != JsonTokenType_String || string_is_empty(token.val_string)) {
            *res = json_err(token.type == JsonTokenType_End ? JsonError_Truncated : JsonError_InvalidFieldName);

            return input;
        }

        const JsonVal fieldName = json_add_string(doc, token.val_string);

        input = json_lex(input, &token);
        if (token.type != JsonTokenType_Colon) {
            *res = json_err(JsonError_InvalidFieldSeperator);

            return input;
        }

        input = json_read(doc, input, &valRes);
        if (valRes.type == JsonResultType_Fail) {
            *res = json_err(valRes.error);

            return input;
        }

        if (!json_add_field(doc, object, fieldName, valRes.val)) {
            *res = json_err(JsonError_DuplicateField);

            return input;
        }

        input = json_lex(input, &token);
        switch (token.type) {
            case JsonTokenType_CurlyClose: {
                goto Success;
            }

            case JsonTokenType_Comma: {
            
            } break;

            case JsonTokenType_End: {
                *res = json_err(JsonError_Truncated);
            } break;

            case JsonTokenType_Error: {
                *res = json_err(token.val_error);
            } break;

            default: {
                *res = json_err(JsonError_UnexpectedToken);

                return input;
            }
        }
    }

Success:
    *res = json_success(object);

    return input;
}

static String json_read_with_start_token(JsonDoc* doc, String input, JsonToken startToken, JsonResult* res) {
    static THREAD_LOCAL u32 depth;
    if (++depth > json_depth_max) {
        *res = json_err(JsonError_MaximumDepthExceeded);
        --depth;

        return input;
    }

    switch (startToken.type) {
        case JsonTokenType_BracketOpen: {
            input = json_read_array(doc, input, res);
        } break;

        case JsonTokenType_CurlyOpen: {
            input = json_read_object(doc, input, res);
        } break;

        case JsonTokenType_BracketClose:
        case JsonTokenType_CurlyClose:
        case JsonTokenType_Comma:
        case JsonTokenType_Colon: {
            *res = json_err(JsonError_UnexpectedToken);
        } break;

        case JsonTokenType_String: {
            *res = json_success(json_add_string(doc, startToken.val_string));
        } break;

        case JsonTokenType_Number: {
            *res = json_success(json_add_number(doc, startToken.val_number));
        } break;

        case JsonTokenType_True: {
            *res = json_success(json_add_bool(doc, true));
        } break;

        case JsonTokenType_False: {
            *res = json_success(json_add_bool(doc, false));
        } break;

        case JsonTokenType_Null: {
            *res = json_success(json_add_null(doc));
        } break;

        case JsonTokenType_Error: {
            *res = json_err(startToken.val_error);
        } break;

        case JsonTokenType_End: {
            *res = json_err(JsonError_Truncated);
        } break;
    }

    --depth;

    return input;
}

String json_read(JsonDoc* doc, String input, JsonResult* res) {
    JsonToken startToken;
    input = json_lex(input, &startToken);

    return json_read_with_start_token(doc, input, startToken, res);
}