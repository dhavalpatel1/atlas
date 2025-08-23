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

static i64 g_perfCounterFrequency;

void time_pal_init() {
    LARGE_INTEGER freq;
    if (LIKELY(QueryPerformanceFrequency(&freq))) {
        g_perfCounterFrequency = freq.QuadPart;
    } else {
        g_perfCounterFrequency = 1;
    }
}

TimeSteady time_pal_steady_clock() {
    LARGE_INTEGER perfTicks;
    const BOOL res = QueryPerformanceCounter(&perfTicks);
    if (UNLIKELY(!res)) {
        diag_crash_msg("QueryPerformanceCounter() failed");
    }

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
            return -(TimeZone)(timeZoneInfo.Bias + timeZoneInfo.StandardBias);
        }

        case TIME_ZONE_ID_DAYLIGHT: {
            return -(TimeZone)(timeZoneInfo.Bias + timeZoneInfo.DaylightBias);
        }

        case TIME_ZONE_ID_INVALID:
        default: {
            diag_crash_msg("GetTimeZoneInformation() failed");
        }
    }
}

TimeReal time_pal_native_to_real(const struct _FILETIME* fileTime) {
    const i64 winEpochToUnixEpoch = i64_lit(116444736000000000);
    const i64 winTickToMicro = i64_lit(10);

    LARGE_INTEGER winTicks;
    winTicks.LowPart = fileTime->dwLowDateTime;
    winTicks.HighPart = fileTime->dwHighDateTime;

    return (winTicks.QuadPart - winEpochToUnixEpoch) / winTickToMicro;
}