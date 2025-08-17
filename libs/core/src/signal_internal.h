/**
 * @file signal_internal.h
 * @brief Internal signal handling platform abstraction layer interface
 *
 * This header defines the platform abstraction layer interface for signal
 * handling operations including signal handler setup, status checking, and
 * signal state management across different operating systems.
 */

#pragma once

#include "core_signal.h"

void signal_pal_setup_handlers();

bool signal_pal_is_recieved(Signal signal);

void signal_pal_reset(Signal signal);