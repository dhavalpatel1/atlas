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

#if defined(ATLAS_LINUX)
struct timespec;
/**
 * @brief Convert Linux timespec to cross-platform TimeReal
 * @param ts Linux timespec structure containing seconds and nanoseconds
 * @return Time in microseconds since Unix epoch
 * 
 * Converts Linux-specific timespec structure to the cross-platform
 * TimeReal representation for consistent time handling across platforms.
 */
TimeReal time_pal_native_to_real(struct timespec ts);
#elif defined(ATLAS_WIN32)
struct _FILETIME;
/**
 * @brief Convert Windows FILETIME to cross-platform TimeReal
 * @param fileTime Windows FILETIME structure pointer
 * @return Time in microseconds since Unix epoch
 * 
 * Converts Windows FILETIME (100-nanosecond intervals since January 1, 1601)
 * to the cross-platform TimeReal representation (microseconds since Unix epoch).
 */
TimeReal time_pal_native_to_real(const struct _FILETIME* fileTime);
#endif