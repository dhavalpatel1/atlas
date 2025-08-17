/**
 * @file time.c
 * @brief Time and date handling implementation
 *
 * This file implements time measurement and date conversion utilities including
 * steady clock (monotonic), real-time clock (wall clock), date arithmetic,
 * and calendar calculations. It provides cross-platform time functionality
 * through platform abstraction layers.
 */

#include "core_diag.h"
#include "core_string.h"
#include "core_time.h"

#include "init_internal.h"
#include "time_internal.h"

/** @brief Flag to track whether the time subsystem has been initialized */
static bool g_initialized;

/**
 * @brief Calculate the number of days since Unix epoch for a given time
 * @param time The real time value to convert
 * @return Number of days since January 1, 1970
 */
static i32 time_days_since_epoch(const TimeReal time) {
    return time / (time_day / time_microsecond);
}

void time_init() {
    time_pal_init();
    g_initialized = true;
}

TimeSteady time_steady_clock() {
    diag_assert_msg(g_initialized, "Time subsystemis not initialized, call core_init() at startup");

    return time_pal_steady_clock();
}

TimeDuration time_steady_duration(const TimeSteady from, const TimeSteady to) {
    return to - from;
}

TimeReal time_real_clock() {
    diag_assert_msg(g_initialized, "Time subsystemis not initialized, call core_init() at startup");
    
    return time_pal_real_clock();
}

TimeDuration time_real_duration(const TimeReal from, const TimeReal to) {
    return time_microseconds(to - from);
}

TimeReal time_real_offset(const TimeReal time, const TimeDuration duration) {
    return time + (duration / time_microsecond);
}

TimeWeekDay time_real_to_weekday(const TimeReal time) {
    return (time_days_since_epoch(time) + TimeWeekDay_Thursday) % 7;
}

TimeDate time_real_to_date(const TimeReal time) {
    // Convert to days since epoch and adjust to internal calendar epoch
    const i64 z = time_days_since_epoch(time) + 719468;
    
    // Calculate 400-year era (handles leap year cycles efficiently)
    const i32 era = (z >= 0 ? z : z - 146096) / 146097;
    
    // Day within the current 400-year era
    const i32 dayOfEra = z - era * 146097;
    
    // Year within the era using optimized formula
    const i32 yearOfEra = (dayOfEra - dayOfEra / 1460 + dayOfEra / 36524 - dayOfEra / 146096) / 365;
    
    // Absolute year number
    const i32 year = yearOfEra + era * 400;
    
    // Day within the year (0-based)
    const i32 dayOfYear = dayOfEra - (365 * yearOfEra + yearOfEra / 4 - yearOfEra / 100);
    
    // Month calculation using mathematical formula (avoids lookup tables)
    const i32 mp = (5 * dayOfYear + 2) / 153;
    
    // Day of month (1-based)
    const i32 day = dayOfYear - (153 * mp + 2) / 5 + 1;
    
    // Convert internal month number to standard calendar month
    const TimeMonth month = mp + (mp < 10 ? 3 : -9);

    return (TimeDate) {
        .year = year + (month <= 2),  // Adjust year if in Jan/Feb
        .month = month,
        .day = day
    };
}

TimeReal time_date_to_real(const TimeDate date) {
    // Adjust year for internal calendar (March-based year boundary)
    const i32 year = date.year - (date.month <= 2);
    
    // Calculate which 400-year era this year falls into
    const i32 era = (year >= 0 ? year : year - 399) / 400;
    
    // Year within the current 400-year era
    const i32 yearOfEra = year - era * 400;
    
    // Day of year (0-based) using inverse month formula
    const i32 dayOfYear = (153 * (date.month + (date.month > 2 ? -3 : 9)) + 2) / 5 + (date.day - 1);
    
    // Day within the 400-year era
    const i32 dayOfEra = yearOfEra * 365 + yearOfEra / 4 - yearOfEra / 100 + dayOfYear;
    
    // Absolute day number since internal epoch, then adjust to Unix epoch
    const i32 daysSinceEpoch = era * 146097 + dayOfEra - 719468;

    // Convert days to microseconds (TimeReal unit)
    return (i64)daysSinceEpoch * (time_day / time_microsecond);
}

TimeZone time_zone_current() {
    diag_assert_msg(g_initialized, "Time subsystem is not initialized, call core_init() at startup");

    return time_pal_zone_current();
}

TimeDuration time_zone_to_duration(const TimeZone timezone) {
    return time_minutes(timezone);
}