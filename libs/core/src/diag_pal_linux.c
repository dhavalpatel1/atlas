/**
 * @file diag_pal_linux.c
 * @brief Linux-specific diagnostic system implementation
 *
 * This file implements the platform abstraction layer for diagnostic functions
 * on Linux systems. It uses POSIX signals for breakpoint handling and process
 * termination functions for crash handling.
 */

#include "diag_internal.h"

#include <signal.h>
#include <unistd.h>

/** @brief Flag to track debugger presence during breakpoint handling */
static bool g_debugger_present;

/**
 * @brief Signal handler for SIGTRAP to detect debugger presence
 * @param signum Signal number (unused)
 *
 * This handler is called when a SIGTRAP signal is raised. If no debugger
 * is attached, this handler will be called and can reset the signal handling.
 */
static void diag_sigtrap_handler(int signum) {
    (void)signum;

    g_debugger_present = false;
    signal(SIGTRAP, SIG_DFL);
}

void diag_pal_break() {
    g_debugger_present = true;
    signal(SIGTRAP, diag_sigtrap_handler);
    raise(SIGTRAP);
}

void diag_pal_crash() {
    _exit(1);
}