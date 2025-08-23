
#include "core_diag.h"
#include "core_format.h"
#include "core_signal.h"
#include "core_thread.h"
#include "signal_internal.h"

#include <signal.h>

static i64 g_signal_states[Signal_Count];

static void signal_pal_interupt_handler(int signal) {
    thread_atomic_store_i64(&g_signal_states[Signal_Interupt], 1);
    (void) signal;
}

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