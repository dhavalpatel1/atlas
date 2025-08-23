#pragma once

#include "core_types.h"

typedef i64 TimeDuration;

typedef i64 TimeSteady;

typedef i64 TimeReal;

typedef i16 TimeZone;

typedef enum {
    TimeWeekDay_Monday = 0,
    TimeWeekDay_Tuesday,
    TimeWeekDay_Wednesday,
    TimeWeekDay_Thursday,
    TimeWeekDay_Friday,
    TimeWeekDay_Saturday,
    TimeWeekDay_Sunday,
} TimeWeekDay;

typedef enum {
    TimeMonth_January = 1,
    TimeMonth_February,
    TimeMonth_March,
    TimeMonth_April,
    TimeMonth_May,
    TimeMonth_June,
    TimeMonth_July,
    TimeMonth_August,
    TimeMonth_September,
    TimeMonth_October,
    TimeMonth_November,
    TimeMonth_December,
} TimeMonth;

typedef struct {
    i32 year;
    TimeMonth month;
    u8 day;
} TimeDate;

#define time_nanosecond ((TimeDuration)1)

#define time_microsecond (time_nanosecond * 1000)

#define time_millisecond (time_microsecond * 1000)

#define time_second (time_millisecond * 1000)

#define time_minute (time_second * 60)

#define time_hour (time_minute * 60)

#define time_day (time_hour * 24)

#define time_nanoseconds(_COUNT_) (time_nanosecond * (_COUNT_))

#define time_microseconds(_COUNT_) (time_microsecond * (_COUNT_))

#define time_milliseconds(_COUNT_) (time_millisecond * (_COUNT_))

#define time_seconds(_COUNT_) (time_second * (_COUNT_))

#define time_minutes(_COUNT_) (time_minute * (_COUNT_))

#define time_hours(_COUNT_) (time_hour * (_COUNT_))

#define time_days(_COUNT_) (time_day * (_COUNT_))

#define time_real_epoch ((TimeReal)0LL)

#define time_zone_utc ((TimeZone)0)

TimeSteady time_steady_clock();

TimeDuration time_steady_duration(TimeSteady from, TimeSteady to);

TimeReal time_real_clock();

TimeDuration time_real_duration(TimeReal from, TimeReal to);

TimeReal time_real_offset(TimeReal time, TimeDuration offset);

TimeWeekDay time_real_to_weekday(TimeReal time);

TimeDate time_real_to_date(TimeReal time);

TimeReal time_date_to_real(TimeDate date);

TimeZone time_zone_current();

TimeDuration time_zone_to_duration(TimeZone zone);