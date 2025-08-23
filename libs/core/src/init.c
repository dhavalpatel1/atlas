#include "core_init.h"
#include "core_types.h"
#include "core_annotation.h"
#include "core_thread.h"
#include "init_internal.h"

static bool g_initialized;

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