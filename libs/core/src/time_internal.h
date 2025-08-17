/**
 * @file time_internal.h
 * @brief Internal time handling platform abstraction layer interface
 *
 * This header defines the platform abstraction layer interface for time
 * operations including steady clock, real-time clock, and timezone handling
 * across different operating systems.
 */

#pragma once

#include "core_time.h"

void time_pal_init();

TimeSteady time_pal_steady_clock();

TimeReal time_pal_real_clock();

TimeZone time_pal_zone_current();