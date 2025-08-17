/**
 * @file core_time.h
 * @brief Time measurement and utilities for precise timing operations
 *
 * This header provides comprehensive time handling capabilities including
 * high-resolution timing, date/time conversion, duration calculations,
 * and timezone support. It supports both monotonic steady clocks for
 * precise interval measurement and real-time clocks for calendar operations.
 */

#pragma once

#include "core_types.h"

/** @brief Time duration type representing nanoseconds */
typedef i64 TimeDuration;

/** @brief Steady clock time type for monotonic time measurement */
typedef i64 TimeSteady;

/** @brief Real-time clock type for calendar time operations */
typedef i64 TimeReal;

/** @brief Timezone offset type representing minutes from UTC */
typedef i16 TimeZone;

/**
 * @brief Enumeration of days of the week
 */
typedef enum {
    TimeWeekDay_Monday = 0,     /**< Monday */
    TimeWeekDay_Tuesday,        /**< Tuesday */
    TimeWeekDay_Wednesday,      /**< Wednesday */
    TimeWeekDay_Thursday,       /**< Thursday */
    TimeWeekDay_Friday,         /**< Friday */
    TimeWeekDay_Saturday,       /**< Saturday */
    TimeWeekDay_Sunday,         /**< Sunday */
} TimeWeekDay;

/**
 * @brief Enumeration of months of the year
 */
typedef enum {
    TimeMonth_January = 1,      /**< January */
    TimeMonth_February,         /**< February */
    TimeMonth_March,            /**< March */
    TimeMonth_April,            /**< April */
    TimeMonth_May,              /**< May */
    TimeMonth_June,             /**< June */
    TimeMonth_July,             /**< July */
    TimeMonth_August,           /**< August */
    TimeMonth_September,        /**< September */
    TimeMonth_October,          /**< October */
    TimeMonth_November,         /**< November */
    TimeMonth_December,         /**< December */
} TimeMonth;

/**
 * @brief Structure representing a calendar date
 */
typedef struct {
    i32 year;           /**< Year (e.g., 2023) */
    TimeMonth month;    /**< Month of the year */
    u8 day;             /**< Day of the month (1-31) */
} TimeDate;

/** @brief Duration constant representing one nanosecond */
#define time_nanosecond ((TimeDuration)1)

/** @brief Duration constant representing one microsecond (1000 nanoseconds) */
#define time_microsecond (time_nanosecond * 1000)

/** @brief Duration constant representing one millisecond (1000 microseconds) */
#define time_millisecond (time_microsecond * 1000)

/** @brief Duration constant representing one second (1000 milliseconds) */
#define time_second (time_millisecond * 1000)

/** @brief Duration constant representing one minute (60 seconds) */
#define time_minute (time_second * 60)

/** @brief Duration constant representing one hour (60 minutes) */
#define time_hour (time_minute * 60)

/** @brief Duration constant representing one day (24 hours) */
#define time_day (time_hour * 24)

/**
 * @brief Create a duration representing the specified number of nanoseconds
 * @param _COUNT_ Number of nanoseconds
 * @return TimeDuration representing the specified nanoseconds
 */
#define time_nanoseconds(_COUNT_) (time_nanosecond * (_COUNT_))

/**
 * @brief Create a duration representing the specified number of microseconds
 * @param _COUNT_ Number of microseconds
 * @return TimeDuration representing the specified microseconds
 */
#define time_microseconds(_COUNT_) (time_microsecond * (_COUNT_))

/**
 * @brief Create a duration representing the specified number of milliseconds
 * @param _COUNT_ Number of milliseconds
 * @return TimeDuration representing the specified milliseconds
 */
#define time_milliseconds(_COUNT_) (time_millisecond * (_COUNT_))

/**
 * @brief Create a duration representing the specified number of seconds
 * @param _COUNT_ Number of seconds
 * @return TimeDuration representing the specified seconds
 */
#define time_seconds(_COUNT_) (time_second * (_COUNT_))

/**
 * @brief Create a duration representing the specified number of minutes
 * @param _COUNT_ Number of minutes
 * @return TimeDuration representing the specified minutes
 */
#define time_minutes(_COUNT_) (time_minute * (_COUNT_))

/**
 * @brief Create a duration representing the specified number of hours
 * @param _COUNT_ Number of hours
 * @return TimeDuration representing the specified hours
 */
#define time_hours(_COUNT_) (time_hour * (_COUNT_))

/**
 * @brief Create a duration representing the specified number of days
 * @param _COUNT_ Number of days
 * @return TimeDuration representing the specified days
 */
#define time_days(_COUNT_) (time_day * (_COUNT_))

/** @brief TimeReal constant representing the Unix epoch (January 1, 1970) */
#define time_real_epoch ((TimeReal)0LL)

/** @brief TimeZone constant representing UTC (zero offset) */
#define time_zone_utc ((TimeZone)0)

/**
 * @brief Get the current steady clock time for precise interval measurement
 * @return Current steady clock time value
 */
TimeSteady time_steady_clock();

/**
 * @brief Calculate duration between two steady clock time points
 * @param from Starting steady clock time
 * @param to Ending steady clock time
 * @return Duration between the two time points
 */
TimeDuration time_steady_duration(TimeSteady from, TimeSteady to);

/**
 * @brief Get the current real-time clock value
 * @return Current real-time clock value (nanoseconds since Unix epoch)
 */
TimeReal time_real_clock();

/**
 * @brief Calculate duration between two real-time clock points
 * @param from Starting real-time clock value
 * @param to Ending real-time clock value
 * @return Duration between the two time points
 */
TimeDuration time_real_duration(TimeReal from, TimeReal to);

/**
 * @brief Apply a time offset to a real-time clock value
 * @param time Base real-time clock value
 * @param offset Duration to add or subtract
 * @return Adjusted real-time clock value
 */
TimeReal time_real_offset(TimeReal time, TimeDuration offset);

/**
 * @brief Convert real-time clock value to day of the week
 * @param time Real-time clock value to convert
 * @return Day of the week corresponding to the given time
 */
TimeWeekDay time_real_to_weekday(TimeReal time);

/**
 * @brief Convert real-time clock value to calendar date
 * @param time Real-time clock value to convert
 * @return Calendar date corresponding to the given time
 */
TimeDate time_real_to_date(TimeReal time);

/**
 * @brief Convert calendar date to real-time clock value
 * @param date Calendar date to convert
 * @return Real-time clock value corresponding to the given date
 */
TimeReal time_date_to_real(TimeDate date);

/**
 * @brief Get the current system timezone offset
 * @return Current timezone offset in minutes from UTC
 */
TimeZone time_zone_current();

/**
 * @brief Convert timezone offset to duration
 * @param zone Timezone offset in minutes from UTC
 * @return Duration representing the timezone offset
 */
TimeDuration time_zone_to_duration(TimeZone zone);