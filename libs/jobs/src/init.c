#include "core_diag.h"
#include "core_thread.h"

#include "init_internal.h"

#include "jobs_init.h"

static bool g_initialized;

void jobs_init() {
    diag_assert(g_thread_tid == g_thread_main_tid);

    if (!g_initialized) {
        g_initialized = true;

        scheduler_init();
        executor_init();
    }
}

void jobs_teardown() {
    diag_assert(g_thread_tid == g_thread_main_tid);
    
    if (g_initialized) {
        g_initialized = false;

        executor_teardown();
        scheduler_teardown();
    }
}