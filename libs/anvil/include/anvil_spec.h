#pragma once

#include "core_annotation.h"
#include "core_format.h"
#include "core_math.h"
#include "core_sourceloc.h"
#include "core_string.h"

typedef enum {
    AnvilTestFlags_None = 0,
    AnvilTestFlags_Skip = 1 << 0,
    AnvilTestFlags_Focus = 1 << 1
} AnvilTestFlags;

typedef u32 AnvilTestId;

typedef struct sAnvilSpecContext AnvilSpecContext;

typedef struct sAnvilTestContext AnvilTestContext;

typedef struct {
    AnvilTestId id;
    String description;
    SourceLoc source;
    AnvilTestFlags flags;
} AnvilTest;

#define spec_name(_NAME_) _anvil_spec_##_NAME_

#define spec(_NAME_) void spec_name(_NAME_)(MAYBE_UNUSED AnvilSpecContext* _specCtx)

#define skip_it(_DESCRIPTION_, ...) it(_DESCRIPTION_, .flags = AnvilTestFlags_Skip, __VA_ARGS__)

#define focus_it(_DESCRIPTION_, ...) it(_DESCRIPTION_, .flags = AnvilTestFlags_Focus, __VA_ARGS__)

#define setup() if(anvil_visit_setup(_specCtx))

#define teardown() if(anvil_visit_teardown(_specCtx))

#define it(_DESCRIPTION_, ...)                          \
    for (AnvilTestContext* _testCtx = anvil_visit_test( \
        _specCtx,                                       \
        (AnvilTest) {                                   \
            .id = (AnvilTestId)(__COUNTER__),           \
            .description = string_lit(_DESCRIPTION_),   \
            .source = source_location(),                \
            __VA_ARGS__                                 \
        });                                             \
        _testCtx;                                       \
        _testCtx = null)                                \

#define anvil_early_out() anvil_finish(_testCtx)

#define anvil_error(_MSG_FORMAT_LIT_, ...) anvil_report_error(_testCtx, fmt_write_scratch(_MSG_FORMAT_LIT_, __VA_ARGS__), source_location())

#define anvil_msg(_CONDITION_, _MSG_FORMAT_LIT_, ...)   \
    do {                                                \
        if (UNLIKELY(!(_CONDITION_))) {                  \
            anvil_error(_MSG_FORMAT_LIT_, __VA_ARGS__); \
        }                                               \
    } while (false)

#define anvil_require_msg(_CONDITION_, _MSG_FORMAT_LIT_, ...) \
    do {                                                      \
        if (UNLIKELY(!(_CONDITION_))) {                       \
            anvil_error(_MSG_FORMAT_LIT_, __VA_ARGS__);       \
            anvil_early_out();                                \
        }                                                     \
    } while (false)

#define anvil(_CONDITION_) anvil_msg(_CONDITION_, #_CONDITION_)

#define anvil_require(_CONDITION_) anvil_require_msg(_CONDITION_, #_CONDITION_)

#define anvil_eq_int(_A_, _B_) anvil_msg(_A_ == _B_, "{} == {}", fmt_int(_A_), fmt_int(_B_))

#define anvil_neq_int(_A_, _B_) anvil_msg(_A_ != _B_, "{} != {}", fmt_int(_A_), fmt_int(_B_))

#define anvil_eq_float(_A_, _B_, _THRESHOLD_) \
    anvil_msg(math_abs(_A_ - _B_) <= (_THRESHOLD_), "{} == {}", fmt_float(_A_), fmt_float(_B_))

#define anvil_eq_string(_A_, _B_)                                    \
    anvil_msg(                                                       \
        string_eq(_A_, _B_),                                         \
        "'{}' == '{}'",                                              \
        fmt_text(_A_, .flags = FormatTextFlags_EscapeNonPrintAscii), \
        fmt_text(_B_, .flags = FormatTextFlags_EscapeNonPrintAscii))

bool anvil_visit_setup(AnvilSpecContext*);

bool anvil_visit_teardown(AnvilSpecContext*);

AnvilTestContext* anvil_visit_test(AnvilSpecContext*, AnvilTest);

void anvil_report_error(AnvilTestContext*, String msg, SourceLoc);

NORETURN void anvil_finish(AnvilTestContext*);