#pragma once

#include "anvil_runner.h"

#include "spec_internal.h"

typedef struct sAnvilOutput AnvilOutput;

struct sAnvilOutput {
    void (*runStarted)(AnvilOutput*);

    void (*testDiscovered)(AnvilOutput*, usize count, TimeDuration);

    void (*testFinished)(AnvilOutput*, const AnvilSpec*, const AnvilTest*, AnvilResultType, AnvilResult*);

    void (*runFinished)(AnvilOutput*, AnvilResultType, TimeDuration, usize numPassed, usize numFailed, usize numSkipped);

    void (*destroy)(AnvilOutput*);
};

void anvil_output_destroy(AnvilOutput*);