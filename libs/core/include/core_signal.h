/**
 * @file core_signal.h
 * @brief Signal handling utilities for managing system signals
 *
 * This header provides utilities for handling system signals in a cross-platform
 * manner. It allows applications to register signal handlers and respond to
 * system events such as interrupts, termination requests, and other signals.
 */

#pragma once
#include "core_types.h"

/**
 * @brief Enumeration of supported system signals
 */
typedef enum {
    Signal_Interupt = 0,    /**< Interrupt signal (SIGINT) */
    Signal_Count,           /**< Number of supported signals */
} Signal;

/**
 * @brief Check if a specific signal has been received
 * @param signal Signal type to check
 * @return true if the signal has been received, false otherwise
 */
bool signal_is_recieved(Signal signal);

/**
 * @brief Reset the received status of a signal
 * @param signal Signal type to reset
 */
void signal_reset(Signal signal);