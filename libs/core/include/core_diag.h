#pragma once

#include "core_annotation.h"
#include "core_format.h"
#include "core_sourceloc.h"
#include "core_types.h"

typedef bool (*AssertHandler)(String msg, SourceLoc loc, void* context);

#define diag_assert_msg(_CONDITION_, _MSG_FORMAT_LIT_, ...)   \
    do {                                                      \
        if (UNLIKELY(!(_CONDITION_))) {                       \
            diag_assert_fail(_MSG_FORMAT_LIT_, __VA_ARGS__);  \
        }                                                     \
    } while (false)

#define diag_assert(_CONDITION_) diag_assert_msg(_CONDITION_, #_CONDITION_)

#define diag_print(_MSG_FORMAT_LIT_, ...) diag_print_raw(fmt_write_scratch(_MSG_FORMAT_LIT_, __VA_ARGS__))

#define diag_print_err(_MSG_FORMAT_LIT_, ...) diag_print_err_raw(fmt_write_scratch(_MSG_FORMAT_LIT_, __VA_ARGS__))

#define diag_assert_fail(_MSG_FORMAT_LIT_, ...) diag_assert_report_fail(fmt_write_scratch(_MSG_FORMAT_LIT_, __VA_ARGS__), source_location())

#define diag_crash_msg(_MSG_FORMAT_LIT_, ...) diag_crash_msg_raw(fmt_write_scratch(_MSG_FORMAT_LIT_, __VA_ARGS__))

void diag_print_raw(String msg);

void diag_print_err_raw(String msg);

void diag_assert_report_fail(String msg, SourceLoc loc);

void diag_break();

NORETURN void diag_crash();

NORETURN void diag_crash_msg_raw(String msg);

void diag_set_assert_handler(AssertHandler handler, void* context);
