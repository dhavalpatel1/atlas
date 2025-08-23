
#pragma once

#include "core_signal.h"

void signal_pal_setup_handlers();

bool signal_pal_is_recieved(Signal signal);

void signal_pal_reset(Signal signal);