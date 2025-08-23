#pragma once

#include "core_dynarray.h"

#include "cli_app.h"

typedef enum {
    CliOptionType_Flag,
    CliOptionType_Arg,
} CliOptionType;

typedef struct {
    u8 character;
    String name;
} CliFlag;

typedef struct {
    u16 position;
    String name;
} CliArg;

typedef struct {
    CliId a, b;
} CliExclusion;

typedef struct {
    CliOptionType type;
    CliOptionFlags flags;
    CliValidateFunc validator;
    String desc;

    union {
        CliFlag dataFlag;
        CliArg dataArg;
    };
} CliOption;

struct sCliApp {
    String name;
    String desc;
    DynArray options;
    DynArray exclusions;
    Allocator* alloc;
};

CliOption* cli_option(const CliApp*, CliId);

String cli_option_name(const CliApp*, CliId);

CliId cli_find_by_character(const CliApp*, u8 character);

CliId cli_find_by_name(const CliApp*, String name);

CliId cli_find_by_position(const CliApp*, u16 position);