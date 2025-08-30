#include "core_init.h"
#include "core_types.h"
#include "core_annotation.h"
#include "core_thread.h"
#include "init_internal.h"

static bool g_initialized;
static THREAD_LOCAL bool g_initialized_thread;

void core_init() {
    if (!g_initialized) {
        alloc_init();
        thread_init();
        time_init();
    }

    if (!g_initialized_thread) {
        alloc_init_thread();
        thread_init_thread();
        rng_init_thread();
    }

    if (!g_initialized) {
        file_init();
        tty_init();
        signal_init();
        path_init();
    }

    g_initialized = true;
    g_initialized_thread = true;
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
