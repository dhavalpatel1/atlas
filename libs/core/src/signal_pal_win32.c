#include "core_diag.h"
#include "core_signal.h"
#include "core_string.h"
#include "core_thread.h"
#include "signal_internal.h"

#include <Windows.h>
#include <consoleapi.h>

static i64 g_signal_states[Signal_Count];

static BOOL signal_pal_interupt_handler(DWORD dwCtrlType) {
    switch (dwCtrlType) {
        case CTRL_C_EVENT:
        case CTRL_BREAK_EVENT: {
            thread_atomic_store_i64(&g_signal_states[Signal_Interupt], 1);
            
            return true;
        }

        default: {
            return false;
        }
    }
}

static void signal_pal_setup_interupt_handlers() {
    BOOL success = SetConsoleCtrlHandler(signal_pal_interupt_handler, true);
    diag_assert_msg(success, "SetConsoleCtrlHandler() failed");

    (void)success;
}

void signal_pal_setup_handlers() {
    signal_pal_setup_interupt_handlers();
}

bool signal_pal_is_recieved(Signal signal) {
    return thread_atomic_load_i64(&g_signal_states[signal]);
}

void signal_pal_reset(Signal signal) {
    thread_atomic_store_i64(&g_signal_states[signal], 0);
}