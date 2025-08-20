/**
 * @file time_pal_win32.c
 * @brief Windows platform abstraction layer for time operations
 *
 * This file implements Windows-specific time measurement using the high-resolution
 * performance counter API and file time conversion utilities. It provides both
 * steady clock (monotonic) and real-time clock functionality on Windows.
 */

#include "core_annotation.h"
#include "core_diag.h"
#include "core_string.h"
#include "core_time.h"
#include "core_types.h"
#include "time_internal.h"

#include <Windows.h>
#include <profileapi.h>
#include <sysinfoapi.h>
#include <timezoneapi.h>
#include <winnt.h>

/** @brief Cached frequency of the Windows performance counter */
static i64 g_perfCounterFrequency;

void time_pal_init() {
    LARGE_INTEGER freq;
    if (LIKELY(QueryPerformanceFrequency(&freq))) {
        g_perfCounterFrequency = freq.QuadPart;
    } else {
        g_perfCounterFrequency = 1;  // Fallback to prevent division by zero
    }
}

TimeSteady time_pal_steady_clock() {
    LARGE_INTEGER perfTicks;
    const BOOL res = QueryPerformanceCounter(&perfTicks);
    if (UNLIKELY(!res)) {
        diag_crash_msg("QueryPerformanceCounter() failed");
    }

    // return (TimeSteady)(((f64)perfTicks.QuadPart * 1000000000.0) / (f64)g_perfCounterFrequency);
    return perfTicks.QuadPart * i64_lit(100000) / g_perfCounterFrequency * i64_lit(10000);
}

TimeReal time_pal_real_clock() {
    FILETIME filetime;
    GetSystemTimePreciseAsFileTime(&filetime);

    return time_pal_native_to_real(&filetime);
}

TimeZone time_pal_zone_current() {
    TIME_ZONE_INFORMATION timeZoneInfo;
    switch (GetTimeZoneInformation(&timeZoneInfo)) {
        case TIME_ZONE_ID_STANDARD: {
            // Standard time: Use base bias + standard bias (usually 0)
            return -(TimeZone)(timeZoneInfo.Bias + timeZoneInfo.StandardBias);
        }

        case TIME_ZONE_ID_DAYLIGHT: {
            // Daylight time: Use base bias + daylight bias (usually -60 minutes)
            return -(TimeZone)(timeZoneInfo.Bias + timeZoneInfo.DaylightBias);
        }

        case TIME_ZONE_ID_INVALID:
        default: {
            // Timezone information unavailable - fallback to UTC
            diag_crash_msg("GetTimeZoneInformation() failed");
        }
    }
}

/**
 * @brief Convert Windows FILETIME to cross-platform TimeReal format
 * @param fileTime Windows FILETIME structure pointer
 * @return Time in microseconds since Unix epoch
 * 
 * Converts Windows FILETIME (100-nanosecond intervals since January 1, 1601)
 * to microseconds since Unix epoch (January 1, 1970). The conversion involves:
 * 1. Combining low and high 32-bit parts into a 64-bit value
 * 2. Subtracting the epoch difference (116444736000000000 100-ns intervals)
 * 3. Converting from 100-nanosecond intervals to microseconds (divide by 10)
 */
TimeReal time_pal_native_to_real(const struct _FILETIME* fileTime) {
    const i64 winEpochToUnixEpoch = i64_lit(116444736000000000);
    const i64 winTickToMicro = i64_lit(10);

    LARGE_INTEGER winTicks;
    winTicks.LowPart = fileTime->dwLowDateTime;
    winTicks.HighPart = fileTime->dwHighDateTime;

    return (winTicks.QuadPart - winEpochToUnixEpoch) / winTickToMicro;
}