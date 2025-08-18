/**
 * @file diag_pal_win32.c
 * @brief Windows-specific diagnostic system implementation
 *
 * This file implements the platform abstraction layer for diagnostic functions
 * on Windows systems. It uses Windows API functions for debugger detection,
 * breakpoint handling, and process termination.
 */

#include "diag_internal.h"

#include <Windows.h>

void diag_pal_break() {
    if (IsDebuggerPresent()) {
        DebugBreak();
    }
}

void diag_pal_crash() {
    ExitProcess(1);
}