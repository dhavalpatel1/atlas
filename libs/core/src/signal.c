#include "core_signal.h"
#include "init_internal.h"
#include "signal_internal.h"

void signal_init() {
    signal_pal_setup_handlers();
}

bool signal_is_recieved(Signal signal) {
    return signal_pal_is_recieved(signal);
}

void signal_reset(Signal signal) {
    signal_pal_reset(signal);
}