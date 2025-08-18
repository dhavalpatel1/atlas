/**
 * @file diag_internal.h
 * @brief Internal diagnostic system platform abstraction layer interface
 *
 * This header defines the platform-specific diagnostic functions that must be
 * implemented by the Platform Abstraction Layer (PAL). These functions provide
 * low-level debugging and crash handling capabilities that are platform-dependent.
 */

#pragma once

#include "core_diag.h"

/**
 * @brief Platform-specific debugger breakpoint trigger
 *
 * This function triggers a debugger breakpoint using platform-specific mechanisms.
 * On systems with debugger support, this will pause execution at the breakpoint.
 * On systems without debugger support, this function may be a no-op.
 */
void diag_pal_break();

/**
 * @brief Platform-specific program termination
 *
 * This function immediately terminates the program using platform-specific
 * mechanisms. It should not return and should ensure the program exits with
 * an appropriate error code indicating abnormal termination.
 */
NORETURN void diag_pal_crash();