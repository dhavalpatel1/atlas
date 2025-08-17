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

/**
 * @brief Convert Windows FILETIME to microseconds since Unix epoch
 *
 * Windows FILETIME represents time as 100-nanosecond intervals since
 * January 1, 1601. This function converts it to microseconds since
 * the Unix epoch (January 1, 1970).
 *
 * @param filetime Pointer to Windows FILETIME structure
 * @return Time in microseconds since Unix epoch
 */
static TimeReal time_pal_filetime_to_microsinceepoch(const FILETIME* filetime) {
    const i64 winEpochToUnixEpoch = i64_lit(116444736000000000);
    const i64 winTickToMicro = i64_lit(10);

    LARGE_INTEGER winTicks;
    winTicks.LowPart = filetime->dwLowDateTime;
    winTicks.HighPart = filetime->dwHighDateTime;

    return (winTicks.QuadPart - winEpochToUnixEpoch) / winTickToMicro;
}

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

    return time_pal_filetime_to_microsinceepoch(&filetime);
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