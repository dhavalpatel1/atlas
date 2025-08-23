#pragma once

#include "core_dynstring.h"
#include "json_doc.h"

typedef enum {
    JsonWriteFlags_None = 0,
    JsonWriteFlags_Pretty = 1 << 0,
} JsonWriteFlags;

typedef struct {
    JsonWriteFlags flags;
    String indent;
    String newline;
} JsonWriteOpts;

#define json_write_opts(...)            \
    ((JsonWriteOpts){                   \
        .flags = JsonWriteFlags_Pretty, \
        .indent = string_lit("  "),     \
        .newline = string_lit("\n"),    \
        __VA_ARGS__                     \
    })

void json_write(DynString* output, const JsonDoc* doc, JsonVal val, const JsonWriteOpts* opts);