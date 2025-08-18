/**
 * @file core_diag.h
 * @brief Diagnostic and debugging utilities for error handling and assertions
 *
 * This header provides comprehensive diagnostic utilities including assertions,
 * error reporting, and crash handling. It supports formatted output for diagnostic
 * messages, customizable assertion handlers, and source location tracking for
 * debugging purposes. The system is designed to help with both development-time
 * debugging and runtime error handling.
 */

#pragma once

#include "core_annotation.h"
#include "core_format.h"
#include "core_sourceloc.h"
#include "core_types.h"

/**
 * @brief Function pointer type for custom assertion handlers
 * @param msg Assertion failure message
 * @param loc Source location where the assertion failed
 * @param context User-provided context data
 * @return true to continue execution, false to abort
 */
typedef bool (*AssertHandler)(String msg, SourceLoc loc, void* context);

/**
 * @brief Assert a condition with a custom formatted message
 * @param _CONDITION_ Expression to evaluate for truthiness
 * @param _MSG_FORMAT_LIT_ Format string literal for the failure message
 * @param ... Arguments for the format string
 */
#define diag_assert_msg(_CONDITION_, _MSG_FORMAT_LIT_, ...)   \
    do {                                                      \
        if (UNLIKELY(!(_CONDITION_))) {                       \
            diag_assert_fail(_MSG_FORMAT_LIT_, __VA_ARGS__);  \
        }                                                     \
    } while (false)

/**
 * @brief Assert a condition with automatic message generation
 * @param _CONDITION_ Expression to evaluate for truthiness
 */
#define diag_assert(_CONDITION_) diag_assert_msg(_CONDITION_, #_CONDITION_)

/**
 * @brief Print a formatted diagnostic message to standard output
 * @param _MSG_FORMAT_LIT_ Format string literal
 * @param ... Arguments for the format string
 */
#define diag_print(_MSG_FORMAT_LIT_, ...) diag_print_raw(fmt_write_scratch(_MSG_FORMAT_LIT_, __VA_ARGS__))

/**
 * @brief Print a formatted error message to standard error
 * @param _MSG_FORMAT_LIT_ Format string literal
 * @param ... Arguments for the format string
 */
#define diag_print_err(_MSG_FORMAT_LIT_, ...) diag_print_err_raw(fmt_write_scratch(_MSG_FORMAT_LIT_, __VA_ARGS__))

/**
 * @brief Report an assertion failure with formatted message
 * @param _MSG_FORMAT_LIT_ Format string literal for the failure message
 * @param ... Arguments for the format string
 */
#define diag_assert_fail(_MSG_FORMAT_LIT_, ...) diag_assert_report_fail(fmt_write_scratch(_MSG_FORMAT_LIT_, __VA_ARGS__), source_location())

/**
 * @brief Crash the program with a formatted message
 * @param _MSG_FORMAT_LIT_ Format string literal for the crash message
 * @param ... Arguments for the format string
 */
#define diag_crash_msg(_MSG_FORMAT_LIT_, ...) diag_crash_msg_raw(fmt_write_scratch(_MSG_FORMAT_LIT_, __VA_ARGS__))

/**
 * @brief Print a raw message to standard output
 * @param msg Message string to print
 */
void diag_print_raw(String msg);

/**
 * @brief Print a raw error message to standard error
 * @param msg Error message string to print
 */
void diag_print_err_raw(String msg);

/**
 * @brief Report an assertion failure with message and source location
 * @param msg Assertion failure message
 * @param loc Source location where the assertion failed
 */
void diag_assert_report_fail(String msg, SourceLoc loc);

/**
 * @brief Trigger a debugger breakpoint for debugging purposes
 */
void diag_break();

/**
 * @brief Immediately crash the program without a message
 */
NORETURN void diag_crash();

/**
 * @brief Crash the program with a raw message
 * @param msg Crash message to display
 */
NORETURN void diag_crash_msg_raw(String msg);

/**
 * @brief Set a custom assertion handler for the diagnostic system
 * @param handler Custom assertion handler function
 * @param context Context pointer passed to the handler
 */
void diag_set_assert_handler(AssertHandler handler, void* context);