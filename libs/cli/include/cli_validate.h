#pragma once

#include "core_string.h"
#include "core_types.h"

typedef bool (*CliValidateFunc)(const String input);

bool cli_validate_i64(const String input);

bool cli_validate_u64(const String input);

bool cli_validate_f64(const String input);