#pragma once

#include "anvil_def.h"

typedef enum {
    AnvilResultType_Pass = 0,
    AnvilResultType_Fail = 1
} AnvilResultType;

typedef enum {
    AnvilRunFlags_None = 0,
    AnvilRunFlags_OutputPassingTests = 1 << 0,
} AnvilRunFlags;

AnvilResultType anvil_run(AnvilDef*, AnvilRunFlags);