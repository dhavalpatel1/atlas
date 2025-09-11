#pragma once

#include "core_array.h"
#include "core_string.h"
#include "core_types.h"

typedef struct sCliInvocation CliInvocation;

typedef u16 CliId;

String cli_read_string(const CliInvocation*, CliId, String defaultVal);

i64 cli_read_i64(const CliInvocation*, CliId, i64 defaultVal);

u64 cli_read_u64(const CliInvocation*, CliId, u64 defaultVal);

f64 cli_read_f64(const CliInvocation*, CliId, f64 defaultVal);

#define cli_read_choice_array(_CLI_INVOCATION_, _CLI_ID_, _CHOICES_ARRAY_, _DEFAULT_VAL_) \
  cli_read_choice((_CLI_INVOCATION_), (_CLI_ID_), (_CHOICES_ARRAY_), array_elems(_CHOICES_ARRAY_), (_DEFAULT_VAL_))

usize cli_read_choice(const CliInvocation*, CliId, const String* choiceStrs, usize choiceCount, usize defaultVal);
