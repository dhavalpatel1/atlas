/**
 * @file anvil_spec.h
 * @brief Anvil test framework specification and assertion macros
 *
 * This header provides the core testing macros and structures for writing test
 * specifications in the Anvil testing framework, including test definitions,
 * assertions, and lifecycle hooks.
 */

#pragma once

#include "core_annotation.h"
#include "core_format.h"
#include "core_math.h"
#include "core_sourceloc.h"
#include "core_string.h"

/**
 * @brief Flags controlling individual test behavior
 */
typedef enum {
    AnvilTestFlags_None = 0,     /**< Default test behavior */
    AnvilTestFlags_Skip = 1 << 0, /**< Skip this test during execution */
    AnvilTestFlags_Focus = 1 << 1 /**< Focus on this test (run only focused tests) */
} AnvilTestFlags;

/**
 * @brief Unique identifier for a test case
 */
typedef u32 AnvilTestId;

/**
 * @brief Opaque structure representing a test specification context
 */
typedef struct sAnvilSpecContext AnvilSpecContext;

/**
 * @brief Opaque structure representing an individual test context
 */
typedef struct sAnvilTestContext AnvilTestContext;

/**
 * @brief Structure representing a single test case
 */
typedef struct {
    AnvilTestId id;          /**< Unique identifier for the test */
    String description;      /**< Human-readable test description */
    SourceLoc source;        /**< Source location where test is defined */
    AnvilTestFlags flags;    /**< Flags controlling test behavior */
} AnvilTest;

/**
 * @brief Internal macro to generate specification function names
 * @param _NAME_ The base name for the specification
 */
#define spec_name(_NAME_) _anvil_spec_##_NAME_

/**
 * @brief Define a test specification function
 * @param _NAME_ The name of the specification
 */
#define spec(_NAME_) void spec_name(_NAME_)(MAYBE_UNUSED AnvilSpecContext* _specCtx)

/**
 * @brief Define a test case that will be skipped during execution
 * @param _DESCRIPTION_ Human-readable description of the test
 * @param ... Additional test configuration options
 */
#define skip_it(_DESCRIPTION_, ...) it(_DESCRIPTION_, .flags = AnvilTestFlags_Skip, __VA_ARGS__)

/**
 * @brief Define a focused test case (only focused tests will run)
 * @param _DESCRIPTION_ Human-readable description of the test
 * @param ... Additional test configuration options
 */
#define focus_it(_DESCRIPTION_, ...) it(_DESCRIPTION_, .flags = AnvilTestFlags_Focus, __VA_ARGS__)

/**
 * @brief Define setup code that runs before each test in the specification
 */
#define setup() if(anvil_visit_setup(_specCtx))

/**
 * @brief Define teardown code that runs after each test in the specification
 */
#define teardown() if(anvil_visit_teardown(_specCtx))

/**
 * @brief Define a test case within a specification
 * @param _DESCRIPTION_ Human-readable description of the test
 * @param ... Additional test configuration options
 */
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

/**
 * @brief Exit the current test early (typically after a critical failure)
 */
#define anvil_early_out() anvil_finish(_testCtx)

/**
 * @brief Report an error with a formatted message
 * @param _MSG_FORMAT_LIT_ Format string literal for the error message
 * @param ... Arguments for the format string
 */
#define anvil_error(_MSG_FORMAT_LIT_, ...) anvil_report_error(_testCtx, fmt_write_scratch(_MSG_FORMAT_LIT_, __VA_ARGS__), source_location())

/**
 * @brief Assert a condition with a custom error message (continues test on failure)
 * @param _CONDITION_ The condition to check
 * @param _MSG_FORMAT_LIT_ Format string literal for the error message
 * @param ... Arguments for the format string
 */
#define anvil_msg(_CONDITION_, _MSG_FORMAT_LIT_, ...)   \
    do {                                                \
        if (UNLIKELY(!(_CONDITION_))) {                  \
            anvil_error(_MSG_FORMAT_LIT_, __VA_ARGS__); \
        }                                               \
    } while (false)

/**
 * @brief Assert a condition with a custom error message (exits test on failure)
 * @param _CONDITION_ The condition to check
 * @param _MSG_FORMAT_LIT_ Format string literal for the error message
 * @param ... Arguments for the format string
 */
#define anvil_require_msg(_CONDITION_, _MSG_FORMAT_LIT_, ...) \
    do {                                                      \
        if (UNLIKELY(!(_CONDITION_))) {                       \
            anvil_error(_MSG_FORMAT_LIT_, __VA_ARGS__);       \
            anvil_early_out();                                \
        }                                                     \
    } while (false)

/**
 * @brief Assert a condition (continues test on failure)
 * @param _CONDITION_ The condition to check
 */
#define anvil(_CONDITION_) anvil_msg(_CONDITION_, #_CONDITION_)

/**
 * @brief Assert a condition (exits test on failure)
 * @param _CONDITION_ The condition to check
 */
#define anvil_require(_CONDITION_) anvil_require_msg(_CONDITION_, #_CONDITION_)

/**
 * @brief Assert that two integers are equal
 * @param _A_ First integer value
 * @param _B_ Second integer value
 */
#define anvil_eq_int(_A_, _B_) anvil_msg(_A_ == _B_, "{} == {}", fmt_int(_A_), fmt_int(_B_))

/**
 * @brief Assert that two integers are not equal
 * @param _A_ First integer value
 * @param _B_ Second integer value
 */
#define anvil_neq_int(_A_, _B_) anvil_msg(_A_ != _B_, "{} != {}", fmt_int(_A_), fmt_int(_B_))

/**
 * @brief Assert that two floating-point values are equal within a threshold
 * @param _A_ First floating-point value
 * @param _B_ Second floating-point value
 * @param _THRESHOLD_ Maximum allowed difference between the values
 */
#define anvil_eq_float(_A_, _B_, _THRESHOLD_) \
    anvil_msg(math_abs(_A_ - _B_) <= (_THRESHOLD_), "{} == {}", fmt_float(_A_), fmt_float(_B_))

/**
 * @brief Assert that two strings are equal
 * @param _A_ First string
 * @param _B_ Second string
 */
#define anvil_eq_string(_A_, _B_)                                    \
    anvil_msg(                                                       \
        string_eq(_A_, _B_),                                         \
        "'{}' == '{}'",                                              \
        fmt_text(_A_, .flags = FormatTextFlags_EscapeNonPrintAscii), \
        fmt_text(_B_, .flags = FormatTextFlags_EscapeNonPrintAscii))

/**
 * @brief Visit the setup phase of a test specification
 * @param context The specification context
 * @return true if setup should be executed, false otherwise
 */
bool anvil_visit_setup(AnvilSpecContext*);

/**
 * @brief Visit the teardown phase of a test specification
 * @param context The specification context
 * @return true if teardown should be executed, false otherwise
 */
bool anvil_visit_teardown(AnvilSpecContext*);

/**
 * @brief Visit and execute a test case
 * @param context The specification context
 * @param test The test case to execute
 * @return The test context for the executed test, or null if test should be skipped
 */
AnvilTestContext* anvil_visit_test(AnvilSpecContext*, AnvilTest);

/**
 * @brief Report an error for the current test
 * @param context The test context
 * @param msg The error message
 * @param source Source location where the error occurred
 */
void anvil_report_error(AnvilTestContext*, String msg, SourceLoc);

/**
 * @brief Finish the current test and exit (used for early test termination)
 * @param context The test context
 */
NORETURN void anvil_finish(AnvilTestContext*);