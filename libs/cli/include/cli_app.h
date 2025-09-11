#pragma once

#include "core_alloc.h"
#include "core_array.h"
#include "core_macro.h"
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

#define cli_register_exclusions(_CLI_APP_, _CLI_ID_, ...)                                                            \
    cli_register_exclusions_raw((_CLI_APP_), (_CLI_ID_), (const CliId[]){ __VA_ARGS__ }, COUNT_VA_ARGS(__VA_ARGS__))

void cli_register_exclusions_raw(CliApp*, CliId id, const CliId* otherIds, usize otherCount);

void cli_register_desc(CliApp*, CliId, String desc);

#define cli_register_desc_choice_array(_CLI_APP_, _CLI_ID_, _DESC_, _CHOICES_ARRAY_, _DEFAULT_CHOICE_)                         \
    cli_register_desc_choice((_CLI_APP_), (_CLI_ID_), (_DESC_), (_CHOICES_ARRAY_), array_elems(_CHOICES_ARRAY_), (_DEFAULT_CHOICE_))

void cli_register_desc_choice(CliApp*, CliId, String desc, const String* choiceStrs, usize choiceCount, usize defaultChoice);

String cli_desc(const CliApp*, CliId);

bool cli_excludes(const CliApp*, CliId a, CliId b);
