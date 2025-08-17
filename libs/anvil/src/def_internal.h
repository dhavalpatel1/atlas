/**
 * @file def_internal.h
 * @brief Internal Anvil definition structures
 *
 * This header defines the internal structures used for Anvil test
 * definition management including spec storage and definition metadata.
 */

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