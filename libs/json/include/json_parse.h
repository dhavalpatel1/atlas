#pragma once

#include "json_doc.h"

typedef enum {
    JsonResultType_Success,
    JsonResultType_Fail,
} JsonResultType;

typedef enum {
    JsonError_DuplicateField,
    JsonError_InvalidChar,
    JsonError_InvalidCharInFalse,
    JsonError_InvalidCharInNull,
    JsonError_InvalidCharInString,
    JsonError_InvalidCharInTrue,
    JsonError_InvalidEscapeSequence,
    JsonError_InvalidFieldName,
    JsonError_InvalidFieldSeperator,
    JsonError_MaximumDepthExceeded,
    JsonError_TooLongString,
    JsonError_Truncated,
    JsonError_UnexpectedToken,
    JsonError_UnterminatedString,

    JsonError_Count,
} JsonError;

typedef struct {
    JsonResultType type;
    union {
        JsonVal val;
        JsonError error;
    };
} JsonResult;

String json_error_str(JsonError);

String json_read(JsonDoc*, String, JsonResult* res);