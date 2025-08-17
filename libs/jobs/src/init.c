/**
 * @file init.c
 * @brief Initialization functions implementation
 *
 * This file implements the initialization and teardown functionality for the
 * jobs library. It manages the startup and shutdown sequence of the scheduler
 * and executor subsystems, ensuring proper resource management and thread safety.
 */

#include "core_diag.h"
#include "core_thread.h"

#include "init_internal.h"

#include "jobs_init.h"

/**
 * @brief Global initialization state flag
 */
static bool g_initialized;

/**
 * @brief Initializes the jobs library
 *
 * Sets up the scheduler and executor subsystems. Must be called from the
 * main thread before any job operations can be performed. Safe to call
 * multiple times - subsequent calls are ignored.
 */
void jobs_init() {
    diag_assert(g_thread_tid == g_thread_main_tid);

    if (!g_initialized) {
        g_initialized = true;

        scheduler_init();
        executor_init();
    }
}

/**
 * @brief Shuts down the jobs library
 *
 * Cleanly tears down the executor and scheduler subsystems, ensuring all
 * resources are freed. Must be called from the main thread. Safe to call
 * multiple times - subsequent calls are ignored.
 */
void jobs_teardown() {
    diag_assert(g_thread_tid == g_thread_main_tid);
    
    if (g_initialized) {
        g_initialized = false;

        executor_teardown();
        scheduler_teardown();
    }
}