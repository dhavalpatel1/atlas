#pragma once

#include "core_dynstring.h"

typedef struct sFile File;

typedef struct sCliApp CliApp;

typedef enum {
    CliHelpFlags_None = 0,
    CliHelpFlags_Style = 1 << 0
} CliHelpFlags;

void cli_help_write(DynString*, CliApp*, CliHelpFlags);

void cli_help_write_file(CliApp*, File* out);