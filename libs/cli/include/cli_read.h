#pragma once

#include "core_string.h"
#include "core_types.h"

typedef struct sCliInvocation CliInvocation;

typedef u16 CliId;

String cli_read_string(CliInvocation*, CliId, String defaultVal);

i64 cli_read_i64(CliInvocation*, CliId, i64 defaultVal);

u64 cli_read_u64(CliInvocation*, CliId, u64 defaultVal);

f64 cli_read_f64(CliInvocation*, CliId, f64 defaultVal);