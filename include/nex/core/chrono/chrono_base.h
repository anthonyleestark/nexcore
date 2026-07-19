/**
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (c) 2026 Anthony Lee Stark. All rights reserved.
 */

#pragma once

#include <chrono>

#include "nex/base/namespace.h"
#include "nex/base/types.h"

NEX_LAYER_NAMESPACE_BEGIN(chrono)

// =======================================================================
// Base chrono foundation types
// =======================================================================

using TimePoint     = NEX_STD chrono::system_clock::time_point;
using Duration      = NEX_STD chrono::system_clock::duration;
using Date          = NEX_STD chrono::year_month_day;
using Year          = NEX_STD chrono::year;
using Years         = NEX_STD chrono::years;
using Month         = NEX_STD chrono::month;
using Months        = NEX_STD chrono::months;
using Day           = NEX_STD chrono::day;
using Days          = NEX_STD chrono::days;
using Weekday       = NEX_STD chrono::weekday;
using Hours         = NEX_STD chrono::hours;
using Minutes       = NEX_STD chrono::minutes;
using Seconds       = NEX_STD chrono::seconds;
using Milliseconds  = NEX_STD chrono::milliseconds;
using SysTime       = NEX_STD chrono::sys_time<Duration>;
using SysDays       = NEX_STD chrono::sys_days;

// =======================================================================
// Chrono utilities
// =======================================================================

// Validate a given hour value (0-23)
constexpr bool isValidHour(int32 hour) noexcept {
    return (hour >= 0 && hour <= 23);
}
// Validate a given minute value (0-59)
constexpr bool isValidMinute(int32 minute) noexcept {
    return (minute >= 0 && minute <= 59);
}
// Validate a given second value (0-59)
constexpr bool isValidSecond(int32 second) noexcept {
    return (second >= 0 && second <= 59);
}
// Validate a given millisecond value (0-999)
constexpr bool isValidMillisecs(int32 millisecs) noexcept {
    return (millisecs >= 0 && millisecs <= 999);
}

// Validate a given day value (1-31)
constexpr bool isValidDay(int32 day) noexcept {
    return (day >= 1 && day <= 31);
}

// Validate a given month value (1-12)
constexpr bool isValidMonth(int32 month) noexcept {
    return (month >= 1 && month <= 12);
}

// Validate a given year value (e.g., 1-9999)
constexpr bool isValidYear(int32 year) noexcept {
    return (year >= 1 && year <= 9999);
}

// Determine if a given year is a leap year according to the Gregorian calendar rules
constexpr bool isLeapYear(int32 year) noexcept {
    return Year(year).is_leap();
}

// Retrieve the number of days in a specific month of a given year, accounting for leap years
constexpr uint32 daysInMonth(int32 year, uint32 month) noexcept {
    if (!isValidYear(year) || !isValidMonth(month)) {
        return 0; // Invalid year or month
    }
    // Create a year_month object
    NEX_STD chrono::year_month ym{NEX_STD chrono::year{year}, NEX_STD chrono::month{month}};
    // Append the `last` specifier to get the last day of that month
    NEX_STD chrono::year_month_day_last ymdl{ym / NEX_STD chrono::last};
    // Cast the day component to an unsigned integer
    return static_cast<unsigned>(ymdl.day());
}

NEX_LAYER_NAMESPACE_END(chrono)
