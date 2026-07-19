/**
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (c) 2026 Anthony Lee Stark. All rights reserved.
 */

#pragma once

#include "nex/base/macros.h"
#include "nex/base/assert_crash.h"
#include "nex/core/text/string.h"
#include "nex/core/chrono/chrono_base.h"
#include "nex/core/chrono/timespan.h"

NEX_LAYER_NAMESPACE_BEGIN(chrono)

/**
 * @class  DayOfWeek
 * @brief  Day of the week enumeration
 * 
 * Represents the seven days of the week in a strongly-typed enumeration.
 * The enumeration values correspond to the standard weekday encoding where
 * Sunday is 0 and Saturday is 6, matching the ISO 8601 standard for
 * weekday representation.
 * 
 * @details
 * The enumeration provides type-safe day-of-week values that can be used
 * with DateTime and other time-related operations. The underlying type is
 * uint8, ensuring compact storage while providing clear semantic meaning.
 * 
 * @note Sunday = 0, Monday = 1, Tuesday = 2, Wednesday = 3, Thursday = 4,
 *       Friday = 5, Saturday = 6
 * @see DateTime
 */
enum class DayOfWeek : uint8 { 
    Sunday, Monday, Tuesday, Wednesday, 
    Thursday, Friday, Saturday 
};

/**
 * @class   Date
 * @brief   Represents a calendar date without a time of day.
 *
 * Date uses the proleptic Gregorian calendar. A default constructed Date is
 * empty; all dates constructed from components are validated. Arithmetic is
 * performed on whole calendar days, so no timezone or time-of-day information
 * can affect the result.
 */
class NEX_API Date {
private:
    CalendarDate date_{};

    // Assign validated date components to the internal representation.
    constexpr void assign(int32 year, uint32 month, uint32 day) noexcept {
        date_ = CalendarDate{ Year{ year }, Month{ month }, Day{ day } };
    }

    // Assert that this Date contains a valid calendar date.
    constexpr void requireValid() const noexcept {
        NEX_ASSERT(ok());
    }

public:
    // Construct an empty (invalid) Date.
    explicit Date() = default;

    // Copy constructor and assignment.
    NEX_DEFAULT_COPY(Date);

    // Move constructor and assignment.
    NEX_DEFAULT_MOVE(Date);

    // Construct a Date from year, month, and day components.
    Date(int32 year, uint32 month, uint32 day) noexcept {
        NEX_ASSERT(isValidDate(year, month, day));
        assign(year, month, day);
    }

    // Construct a Date from strongly typed chrono calendar components.
    Date(Year year, Month month, Day day) noexcept : date_(year, month, day) {
        NEX_ASSERT(isValidDate(static_cast<int32>(year),
                               static_cast<uint32>(month),
                               static_cast<uint32>(day)));
    }

    // Construct a Date from the internal calendar representation.
    explicit constexpr Date(CalendarDate date) noexcept : date_(date) {}

    // Construct a Date from a system-clock day time point.
    explicit constexpr Date(SysDays date) noexcept : date_(date) {}

    // Construct a Date by discarding the time-of-day portion of a time point.
    explicit Date(TimePoint timePoint) noexcept
        : date_(NEX_STD chrono::floor<Days>(timePoint)) {}

    // Get the underlying calendar representation.
    constexpr CalendarDate getCalendarDate() const noexcept {
        return date_;
    }

    // Get the date as a system-clock time point at midnight.
    constexpr SysDays getSysDays() const noexcept {
        return SysDays{ date_ };
    }

    // Get the date as a system-clock time point at midnight using the native duration.
    constexpr TimePoint getTimePoint() const noexcept {
        return TimePoint{ getSysDays() };
    }

    // Check whether this Date is the invalid default value rather than the Unix epoch.
    constexpr bool isEmpty() const noexcept { return !date_.ok(); }

    // Check whether this Date contains valid calendar components.
    constexpr bool ok() const noexcept { return date_.ok(); }

    // Return a Date offset by the whole-day portion of a time span.
    Date operator+(const TimeSpan& timeSpan) const noexcept;

    // Offset this Date by the whole-day portion of a time span.
    Date& operator+=(const TimeSpan& timeSpan) noexcept;

    // Return a Date offset by a number of calendar days.
    Date operator+(int32 days) const noexcept;

    // Offset this Date by a number of calendar days.
    Date& operator+=(int32 days) noexcept;

    // Return the elapsed time span between this Date and another Date.
    TimeSpan operator-(const Date& other) const noexcept;

    // Return a Date moved backward by the whole-day portion of a time span.
    Date operator-(const TimeSpan& timeSpan) const noexcept;

    // Move this Date backward by the whole-day portion of a time span.
    Date& operator-=(const TimeSpan& timeSpan) noexcept;

    // Return a Date moved backward by a number of calendar days.
    Date operator-(int32 days) const noexcept;

    // Move this Date backward by a number of calendar days.
    Date& operator-=(int32 days) noexcept;

    // Compare two Dates for equality.
    constexpr bool operator==(const Date& other) const noexcept { return date_ == other.date_; }

    // Compare two Dates for inequality.
    constexpr bool operator!=(const Date& other) const noexcept { return date_ != other.date_; }

    // Check whether this Date is earlier than another Date.
    constexpr bool operator<(const Date& other) const noexcept { return date_ < other.date_; }

    // Check whether this Date is later than another Date.
    constexpr bool operator>(const Date& other) const noexcept { return other < *this; }

    // Check whether this Date is no later than another Date.
    constexpr bool operator<=(const Date& other) const noexcept { return !(other < *this); }

    // Check whether this Date is no earlier than another Date.
    constexpr bool operator>=(const Date& other) const noexcept { return !(*this < other); }

    // Advance this Date by one calendar day.
    Date& operator++() noexcept { return increaseDays(1); }

    // Move this Date back by one calendar day.
    Date& operator--() noexcept { return decreaseDays(1); }

    // Return the original Date, then advance this Date by one calendar day.
    Date operator++(int32) noexcept { Date original = *this; ++(*this); return original; }

    // Return the original Date, then move this Date back by one calendar day.
    Date operator--(int32) noexcept { Date original = *this; --(*this); return original; }

    // Validate a Gregorian calendar year in the supported range.
    static constexpr bool isValidYear(int32 year) noexcept {
        return chrono::isValidYear(year);
    }

    // Validate a month number in the range 1-12.
    static constexpr bool isValidMonth(uint32 month) noexcept {
        return chrono::isValidMonth(static_cast<int32>(month));
    }

    // Validate a day number in the range 1-31.
    static constexpr bool isValidDay(uint32 day) noexcept {
        return chrono::isValidDay(static_cast<int32>(day));
    }

    // Validate a complete Gregorian calendar date.
    static constexpr bool isValidDate(int32 year, uint32 month, uint32 day) noexcept {
        return isValidYear(year) && isValidMonth(month) && isValidDay(day)
            && CalendarDate{ Year{ year }, Month{ month }, Day{ day } }.ok();
    }

    // Get the year component.
    constexpr int32 year() const noexcept { return static_cast<int32>(date_.year()); }

    // Get the month component in the range 1-12.
    constexpr uint32 month() const noexcept { return static_cast<uint32>(date_.month()); }

    // Get the day-of-month component in the range 1-31.
    constexpr uint32 day() const noexcept { return static_cast<uint32>(date_.day()); }

    // Get the weekday using Sunday = 0 through Saturday = 6.
    constexpr uint8 dayOfWeek() const noexcept {
        return static_cast<uint8>(Weekday{ getSysDays() }.c_encoding());
    }

    // Get the weekday as a strongly typed DayOfWeek value.
    constexpr DayOfWeek weekday() const noexcept {
        return static_cast<DayOfWeek>(dayOfWeek());
    }

    // Check whether this Date belongs to a leap year.
    constexpr bool isLeapYear() const noexcept { return date_.year().is_leap(); }

    // Check whether this Date is the current system date.
    bool isToday() const noexcept;

    // Replace all calendar components with a validated date.
    void setDate(int32 year, uint32 month, uint32 day) noexcept {
        NEX_ASSERT(isValidDate(year, month, day));
        assign(year, month, day);
    }

    // Replace the year while preserving the month and day.
    void setYear(int32 year) noexcept;

    // Replace the month while preserving the year and day.
    void setMonth(uint32 month) noexcept;

    // Replace the day while preserving the year and month.
    void setDay(uint32 day) noexcept;

    // Advance this Date by a number of calendar days.
    Date& increaseDays(int32 days) noexcept;

    // Move this Date back by a number of calendar days.
    Date& decreaseDays(int32 days) noexcept;

    // Advance this Date by a number of calendar months, clamping the day if needed.
    Date& increaseMonths(int32 months) noexcept;

    // Move this Date back by a number of calendar months, clamping the day if needed.
    Date& decreaseMonths(int32 months) noexcept;

    // Advance this Date by a number of calendar years, clamping February 29 if needed.
    Date& increaseYears(int32 years) noexcept;

    // Move this Date back by a number of calendar years, clamping February 29 if needed.
    Date& decreaseYears(int32 years) noexcept;

    /**
     * @brief  Convert Date to a string representation.
     * @param  format  "default" or "iso" for YYYY-MM-DD, "us" for MM/DD/YYYY,
     *                 or "compact" for YYYYMMDD.
     * @return The formatted Date, or an empty String for an empty Date.
     */
     String toString(wcstring format = L"default") const;

     /**
     * @brief  Create a Date from a string representation.
     * @param  str  A YYYY-MM-DD, YYYY/MM/DD, MM/DD/YYYY, or YYYYMMDD date.
     * @return A parsed Date, or an empty Date when parsing or validation fails.
     */
    static Date fromString(const String& str);
};

NEX_LAYER_NAMESPACE_END(chrono)
