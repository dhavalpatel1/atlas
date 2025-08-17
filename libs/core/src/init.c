/**
 * @file init.c
 * @brief Core library initialization and teardown management.
 *
 * This file manages the initialization and teardown of all core library subsystems.
 * It ensures proper initialization order and provides both global and thread-local
 * initialization. The initialization is idempotent and handles both main thread
 * and worker thread setup correctly.
 */

#include "core_init.h"
#include "core_types.h"
#include "core_annotation.h"
#include "core_thread.h"
#include "init_internal.h"

/** @brief Global flag indicating whether the core library has been initialized. */
static bool g_initialized;

/** @brief Thread-local flag indicating whether the current thread has been initialized. */
static THREAD_LOCAL bool g_initialized_thread;

void alloc_init();

void alloc_init_thread();

void alloc_teardown_thread();

void time_init();

void file_init();

void tty_init();

void path_init();

void thread_init();

void thread_init_thread();

void signal_init();

void tty_teardown();

/**
 * @brief Initialize the core library for the current process and thread.
 *
 * This function performs both global (process-wide) and thread-local initialization.
 * Global initialization happens only once per process, while thread-local initialization
 * happens once per thread. The function is safe to call multiple times.
 */
void core_init() {
    if (!g_initialized) {
        g_initialized = true;

        alloc_init();
        time_init();
        file_init();
        tty_init();
        path_init();
        thread_init();
        signal_init();
    }

    if (!g_initialized_thread) {
        alloc_init_thread();
        thread_init_thread();
        rng_init_thread();
        g_initialized_thread = true;
    }
}

/**
 * @brief Tear down the core library for the current thread and optionally the process.
 *
 * This function performs thread-local teardown for all threads and global teardown
 * only when called from the main thread. It ensures proper cleanup of resources
 * and subsystems in the correct order.
 */
void core_teardown() {
    if (g_thread_tid == g_thread_main_tid && g_initialized) {
        g_initialized = false;
        tty_teardown();
    }

    if (g_initialized_thread) {
        alloc_teardown_thread();
        g_initialized_thread = false;
    }
}