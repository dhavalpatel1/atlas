#pragma once

#include "core_dynarray.h"
#include "core_string.h"

#include "anvil_def.h"

typedef struct {
    String name;
    AnvilSpecRoutine routine;
} AnvilSpecDef;

struct sAnvilDef {
    DynArray specs;
    Allocator* alloc;
};