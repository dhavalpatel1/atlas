#pragma once

#include "core_dynstring.h"

typedef struct sFile File;

typedef struct sCliApp CliApp;

typedef struct sCliInvocation CliInvocation;

typedef enum {
    CliFailureFlags_None = 0,
    CliFailureFlags_Style = 1 << 0
} CliFailureFlags;

void cli_failure_write(DynString*, CliInvocation*, CliFailureFlags);

void cli_failure_write_file(CliInvocation*, File*);