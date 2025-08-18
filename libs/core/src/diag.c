/**
 * @file diag.c
 * @brief Diagnostic and assertion handling implementation
 *
 * This file implements diagnostic functionality including assertion handling,
 * error reporting, and crash management. It provides thread-local assertion
 * handlers and various utility functions for debugging and error reporting.
 */

#include "core_diag.h"
#include "core_file.h"
#include "core_format.h"

#include "diag_internal.h"

/** @brief Thread-local custom assertion handler function pointer */
THREAD_LOCAL AssertHandler g_assertHandler;
/** @brief Thread-local context data for the assertion handler */
THREAD_LOCAL void* g_assertHandlerContext;

/**
 * @brief Default assertion handler that prints error messages
 *
 * This is the default handler used when no custom assertion handler is set.
 * It prints assertion failure information including the message, file, and
 * line number to stderr.
 *
 * @param msg The assertion message
 * @param source Source location information (file and line)
 * @param context User context (unused in default handler)
 * @return false to indicate the assertion should cause a crash
 */
static bool assert_handler_print(String msg, const SourceLoc source, void* context) {
    (void)context;

    diag_print_err("Assertion failed: '{}' [file: {} line: {}]\n", fmt_text(msg), fmt_path(source.file), fmt_int(source.line));

    return false;
}

/**
 * @brief Get the current assertion handler
 *
 * Returns the currently active assertion handler, falling back to the
 * default print handler if no custom handler has been set.
 *
 * @return The active assertion handler function
 */
static AssertHandler assert_handler() {
    return g_assertHandler ? g_assertHandler : assert_handler_print;
}

void diag_print_raw(String msg) {
    file_write_sync(g_file_stdout, msg);
}

void diag_print_err_raw(String msg) {
    file_write_sync(g_file_stderr, msg);
}

void diag_assert_report_fail(String msg, const SourceLoc source) {
    if (!assert_handler()(msg, source, g_assertHandlerContext)) {
        diag_crash();
    }
}

void diag_break() {
    diag_pal_break();
}

void diag_crash() {
    diag_break();
    diag_pal_crash();
}

void diag_crash_msg_raw(String msg) {
    diag_print_err("Crash: '{}'\n", fmt_text(msg));
    diag_crash();
}

void diag_set_assert_handler(AssertHandler handler, void* context) {
    g_assertHandler = handler;
    g_assertHandlerContext = context;
}