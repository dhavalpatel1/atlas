#pragma once
#include "core_types.h"

typedef enum {
    Signal_Interupt = 0,
    Signal_Count,
} Signal;

bool signal_is_recieved(Signal signal);

void signal_reset(Signal signal);