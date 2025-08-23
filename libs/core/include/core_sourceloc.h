#pragma once

#include "core_string.h"

typedef struct {
    String file;
    u32 line;
} SourceLoc;

#define source_file() string_lit(__FILE__)

#define source_line() ((u32)(__LINE__))

#define source_location()      \
    ((SourceLoc) {             \
        .file = source_file(), \
        .line = source_line()  \
    })
