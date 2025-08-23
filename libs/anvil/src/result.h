#pragma once

#include "core_dynarray.h"
#include "core_sourceloc.h"
#include "core_time.h"

typedef struct {
    String msg;
    SourceLoc source;
} AnvilError;

typedef struct {
    Allocator* alloc;
    bool finished;
    TimeDuration duration;
    DynArray errors;
} AnvilResult;

AnvilResult* anvil_result_create(Allocator*);

void anvil_result_destroy(AnvilResult*);

void anvil_result_error(AnvilResult*, String msg, SourceLoc);

void anvil_result_finish(AnvilResult*, TimeDuration);