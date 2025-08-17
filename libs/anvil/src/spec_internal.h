/**
 * @file spec_internal.h
 * @brief Internal Anvil specification structures and execution context
 *
 * This header defines the internal structures for Anvil test specification
 * management including test contexts, execution state, and spec definitions.
 */

#pragma once

#include "anvil_spec.h"

#include <setjmp.h>

#include "def_internal.h"

#include "result.h"

typedef enum {
    AnvilSpecContextFlags_None = 0,
    AnvilSpecContextFlags_Setup = 1 << 0,
    AnvilSpecContextFlags_Teardown = 1 << 1
} AnvilSpecContextFlags;

struct sAnvilSpecContext {
    AnvilTestContext* (*visitTest)(AnvilSpecContext*, AnvilTest);
    AnvilSpecContextFlags flags;
};

struct sAnvilTestContext {
    bool started;
    AnvilResult* result;
    jmp_buf finishJumpDest;
};

typedef struct {
    const AnvilSpecDef* def;
    DynArray tests;
    bool focus;
} AnvilSpec;

AnvilSpec anvil_spec_create(Allocator*, const AnvilSpecDef*);

void anvil_spec_destroy(AnvilSpec*);

AnvilResult* anvil_exec_test(Allocator*, const AnvilSpec*, AnvilTestId);