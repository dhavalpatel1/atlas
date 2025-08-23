#pragma once

#include "core_string.h"
#include "core_types.h"

typedef struct sCliApp CliApp;

typedef u16 CliId;

typedef enum {
    CliParseResult_Success = 0,
    CliParseResult_Fail = 1,
} CliParseResult;

typedef struct {
    String* head;
    usize count;
} CliParseErrors;

typedef struct {
    String* head;
    usize count;
} CliParseValues;

typedef struct sCliInvocation CliInvocation;

CliInvocation* cli_parse(const CliApp*, int argc, const char** argv);

void cli_parse_destroy(CliInvocation*);

CliParseResult cli_parse_result(CliInvocation*);

CliParseErrors cli_parse_errors(CliInvocation*);

bool cli_parse_provided(CliInvocation*, CliId);

CliParseValues cli_parse_values(CliInvocation*, CliId);