/**
 * @file signal_pal_linux.c
 * @brief Linux platform abstraction layer for signal handling
 *
 * This file implements Linux-specific signal handling using POSIX signals.
 * It provides a cross-platform interface for handling system signals like
 * SIGINT (Ctrl+C) through the Linux signal() and sigaction() APIs.
 */

#include "core_diag.h"
#include "core_format.h"
#include "core_signal.h"
#include "core_thread.h"
#include "signal_internal.h"

#include <signal.h>

/** @brief Array tracking the state of each signal type */
static i64 g_signal_states[Signal_Count];

/**
 * @brief Signal handler for interrupt signals (SIGINT)
 *
 * This function is called by the Linux kernel when an interrupt signal
 * is received (typically Ctrl+C). It atomically sets the interrupt
 * flag for thread-safe signal state tracking.
 *
 * @param signal The signal number (unused, but required by signal API)
 */
static void signal_pal_interupt_handler(int signal) {
    thread_atomic_store_i64(&g_signal_states[Signal_Interupt], 1);
    (void) signal;
}

/**
 * @brief Set up the interrupt signal handler for SIGINT
 *
 * Registers the interrupt handler function with the Linux signal system
 * using sigaction() for more reliable signal handling. Configures the
 * handler to restart interrupted system calls automatically.
 */
static void signal_pal_setup_interupt_handler() {
    struct sigaction action = (struct sigaction) {
        .sa_handler = signal_pal_interupt_handler,
        .sa_flags = SA_RESTART
    };
    sigemptyset(&action.sa_mask);

    int res = sigaction(SIGINT, &action, null);
    if (UNLIKELY(res != 0)) {
        diag_crash_msg("sigaction() failed: {}", fmt_int(res));
    }
}

void signal_pal_setup_handlers() {
    signal_pal_setup_interupt_handler();
}

bool signal_pal_is_recieved(Signal signal) {
    return thread_atomic_load_i64(&g_signal_states[signal]);
}

void signal_pal_reset(Signal signal) {
    thread_atomic_store_i64(&g_signal_states[signal], 0);
}