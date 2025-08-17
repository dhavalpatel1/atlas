/**
 * @file tty_pal_win32.c
 * @brief Windows platform abstraction layer for terminal/TTY operations
 *
 * This file implements Windows-specific terminal functionality including
 * console mode management, virtual terminal support, and code page handling.
 * It enables ANSI escape sequence support and advanced terminal features
 * on Windows consoles.
 */

#include "core_diag.h"
#include "core_file.h"
#include "core_string.h"
#include "core_tty.h"
#include "file_internal.h"
#include "tty_internal.h"

#include <Windows.h>
#include <consoleapi.h>
#include <consoleapi2.h>
#include <fileapi.h>
#include <winbase.h>
#include <winnls.h>

/**
 * @brief Structure to track console mode overrides
 *
 * Stores the original console mode so it can be restored when the
 * application exits, along with a flag indicating if override is active.
 */
struct ConsoleModeOverride {
    bool enabled;      /**< Whether the override is currently active */
    DWORD original;    /**< Original console mode to restore later */
};

/** @brief Console mode override state for standard input */
static struct ConsoleModeOverride g_consoleModeInputOverride;

/** @brief Console mode override state for standard output */
static struct ConsoleModeOverride g_consoleModeOutputOverride;

/** @brief Console mode override state for standard error */
static struct ConsoleModeOverride g_consoleModeErrorOverride;

/** @brief Original console code page to restore on exit */
static UINT g_consoleCodePageOriginal;

/**
 * @brief Override console input mode to enable virtual terminal features
 *
 * Modifies the Windows console input mode to enable processed input and
 * virtual terminal input sequences. This allows the application to receive
 * enhanced keyboard input including escape sequences.
 *
 * @param file The input file handle (typically stdin)
 * @param override Structure to store the original mode for restoration
 */
static void tty_pal_override_input_mode(File* file, struct ConsoleModeOverride* override) {
    if (GetConsoleMode(file->handle, &override->original)) {
        DWORD newMode = override->original;
        newMode |= 0x0001;  // ENABLE_PROCESSED_INPUT
        newMode |= 0x0200;  // ENABLE_VIRTUAL_TERMINAL_INPUT
        SetConsoleMode(file->handle, newMode);

        override->enabled = true;
    }
}

/**
 * @brief Override console output mode to enable virtual terminal processing
 *
 * Modifies the Windows console output mode to enable virtual terminal
 * processing. This allows the application to use ANSI escape sequences
 * for colors, cursor control, and other terminal features.
 *
 * @param file The output file handle (stdout or stderr)
 * @param override Structure to store the original mode for restoration
 */
static void tty_pal_override_output_mode(File* file, struct ConsoleModeOverride* override) {
    if (GetConsoleMode(file->handle, &override->original)) {
        DWORD newMode = override->original;
        newMode |= 0x0004;  // ENABLE_VIRTUAL_TERMINAL_PROCESSING
        SetConsoleMode(file->handle, newMode);

        override->enabled = true;
    }
}

/**
 * @brief Restore the original console mode
 *
 * Restores the console mode to its original state before the application
 * modified it. This should be called during cleanup to leave the console
 * in the same state it was found.
 *
 * @param file The file handle whose mode should be restored
 * @param override Structure containing the original mode to restore
 */
static void tty_pal_restore_mode(File* file, struct ConsoleModeOverride* override) {
    if (override->enabled) {
        SetConsoleMode(file->handle, override->original);
    }
}

void tty_pal_init() {
    // Override console modes for enhanced functionality
    tty_pal_override_input_mode(g_file_stdin, &g_consoleModeInputOverride);
    tty_pal_override_output_mode(g_file_stdout, &g_consoleModeOutputOverride);
    tty_pal_override_output_mode(g_file_stderr, &g_consoleModeErrorOverride);

    // Configure UTF-8 code page for proper Unicode support
    g_consoleCodePageOriginal = GetConsoleCP();
    SetConsoleCP(CP_UTF8);
}

void tty_pal_teardown() {
    // Restore all console modes to original settings
    tty_pal_restore_mode(g_file_stdin, &g_consoleModeInputOverride);
    tty_pal_restore_mode(g_file_stdout, &g_consoleModeOutputOverride);
    tty_pal_restore_mode(g_file_stderr, &g_consoleModeErrorOverride);
    
    // Restore original console code page
    SetConsoleCP(g_consoleCodePageOriginal);
}

bool tty_pal_isatty(File *file) {
    return GetFileType(file->handle) == FILE_TYPE_CHAR;
}

u16 tty_pal_width(File *file) {
    diag_assert_msg(tty_pal_isatty(file), "Given file is not a tty");

    CONSOLE_SCREEN_BUFFER_INFO csbi;
    const BOOL res = GetConsoleScreenBufferInfo(file->handle, &csbi);
    if (UNLIKELY(!res)) {
        diag_crash_msg("GetConsoleScreenBufferInfo() failed");
    }

    return (u16)(1 + csbi.srWindow.Right - csbi.srWindow.Left);
}

u16 tty_pal_height(File *file) {
    diag_assert_msg(tty_pal_isatty(file), "Given file is not a tty");

    CONSOLE_SCREEN_BUFFER_INFO csbi;
    const BOOL res = GetConsoleScreenBufferInfo(file->handle, &csbi);
    if (UNLIKELY(!res)) {
        diag_crash_msg("GetConsoleScreenBufferInfo() failed");
    }

    return (u16)(1 + csbi.srWindow.Bottom - csbi.srWindow.Top);
}