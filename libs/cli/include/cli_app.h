#pragma once

#include "core_alloc.h"
#include "core_string.h"

typedef bool (*CliValidateFunc)(const String input);

typedef u16 CliId;

typedef enum {
    CliOptionFlags_None = 0,
    CliOptionFlags_Value = 1 << 0,
    CliOptionFlags_MultiValue = (1 << 1) | CliOptionFlags_Value,
    CliOptionFlags_Required = (1 << 2) | CliOptionFlags_Value,
} CliOptionFlags;

typedef struct sCliApp CliApp;

CliApp* cli_app_create(Allocator*, String desc);

void cli_app_destroy(CliApp*);

CliId cli_register_flag(CliApp*, u8 character, String name, CliOptionFlags);

CliId cli_register_arg(CliApp*, String name, CliOptionFlags);

void cli_register_validator(CliApp*, CliId, CliValidateFunc);

void cli_register_exclusion(CliApp*, CliId a, CliId b);

void cli_register_desc(CliApp*, CliId, String desc);