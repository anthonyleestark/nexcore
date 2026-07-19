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
#include "nex/core/chrono/clock_time.h"
#include "nex/core/chrono/date.h"

NEX_LAYER_NAMESPACE_BEGIN(chrono)

/**
 * @class   DateTime
 * @brief   Represents a date and time
 * 
 * DateTime represents a complete point in time, including both calendar date
 * (year, month, day) and clock time (hour, minute, second, millisecond) components.
 * It is the most comprehensive time representation in NEX and serves as the
 * primary type for timestamps, scheduling, and date/time calculations.
 * 
 * @details
 * DateTime provides extensive functionality for working with complete date/time values:
 * 
 * - Construction: Can be created from individual date/time components (year, month, day,
 *   hour, minute, second, millisecond) or directly from a std::chrono::time_point representing 
 *   an absolute point in time.
 * - String parsing: Can be constructed from a string representation of a date and time, supporting
 *   various formats (e.g., "YYYY-MM-DD HH:MM:SS", "YYYY/MM/DD HH:MM:SS.mmm", "YYYY-MM-DDTHH:MM:SSZ" ISO 8601 format).
 * - String formatting: Can be converted to a string representation in different formats.
 * - Arithmetic operations: Supports addition and subtraction with TimeSpan, as well as comparison
 *   with other DateTime instances or integer seconds since the epoch.
 * - Comparison operators: Provides equality, inequality, and relational operators for comparing
 *   DateTime instances.
 * - Accessors: Provides methods to retrieve individual date and time components (year, month, day, hour, 
 *   minute, second, millisecond)
 * - Date-based calculations: Supports operations like adding months or years, calculating the day of the week,
 *   and determining the number of days in a month.
 * - Validation: Ensures that date and time values are valid when constructing from components or parsing from strings.
 * - Supports checking if the DateTime is empty (default constructed) for convenience.
 * - Handles leap years, month boundaries, and other calendar-specific rules correctly in all operations.
 * - Time arithmetic correctly handles changes in month and year when adding or subtracting time spans.
 * - Provides utility methods for common date/time calculations, such as finding the next occurrence of a specific weekday,
 *   or determining the number of days between two dates.
 * 
 * DateTime is particularly useful for:
 * - Timestamps and event logging
 * - Scheduling future events with specific dates and times
 * - Calculating time differences and intervals
 * - Date-based calculations (e.g., "next Monday", "30 days from now")
 * - Calendar operations and date arithmetic
 * - Storing absolute time points for persistence
 * 
 * @note DateTime uses the Gregorian calendar and handles leap years correctly
 * @note All date/time modifications validate input ranges and handle edge cases
 * @note The class supports dates from the standard chrono library's valid range
 * @note Time arithmetic correctly handles month and year boundaries
 * @see TimeSpan, ClockTime
 */
class NEX_API DateTime {
private:
    // Internal representation of the date and time as a time point
    TimePoint timePoint_{};

public:
    // Constructor
    explicit DateTime() = default;

    // Copy constructor and assignment
    NEX_DEFAULT_COPY(DateTime);

    // Move constructor and assignment
    NEX_DEFAULT_MOVE(DateTime);

    ////// Conversion constructors -----------------------------

    // Construct from a time point
    explicit DateTime(TimePoint timepoint) : timePoint_(timepoint) {}

    // Construct from individual date and time components
    DateTime(int32 year, uint32 month, uint32 day, int32 hour, int32 min, int32 sec, int32 millisec = 0) noexcept {
        NEX_ASSERT(isValidDate(year, month, day));
        NEX_ASSERT(isValidTime(hour, min, sec, millisec));
        auto dateVal = CalendarDate{ Year{ year }, Month{ month }, Day{ day } };
        auto timeVal = Hours(hour) + Minutes(min) + Seconds(sec) + Milliseconds(millisec);
        timePoint_ = SysDays{ dateVal } + timeVal;
    }

    ////// Access date and time components ------------------------

    // Get time point value
    TimePoint getTimePoint(void) const noexcept {
        return timePoint_;
    }

    // Get clock-time data
    ClockTime getClockTime(void) const noexcept {
        return ClockTime(hour(), minute(), second(), millisecond());
    }

    // Get the date portion without its time-of-day component.
    constexpr Date getDate(void) const noexcept {
        return Date{ NEX_STD chrono::floor<Days>(timePoint_) };
    }

    // Check if the DateTime is empty (default constructed)
    constexpr bool isEmpty() const noexcept {
        return timePoint_ == TimePoint{};
    }

    ////// Arithmetic operators ------------------------

    // Addition operator with TimeSpan
    TimeSpan operator+(const TimeSpan& timeSpan) const noexcept {
        return TimeSpan(timePoint_.time_since_epoch() + timeSpan.getDuration());
    }

    // Compound assignment for addition with TimeSpan
    DateTime& operator+=(const TimeSpan& timeSpan) {
        timePoint_ += timeSpan.getDuration();
        return *this;
    }

    // Addition with integer seconds
    TimeSpan operator+(int32 seconds) const noexcept {
        return TimeSpan(timePoint_.time_since_epoch() + Seconds{ seconds });
    }

    // Compound assignment for addition with integer seconds
    DateTime& operator+=(int32 seconds) {
        timePoint_ += Seconds{ seconds };
        return *this;
    }

    // Subtraction operator
    TimeSpan operator-(const DateTime& other) const noexcept {
        return TimeSpan(timePoint_ - other.timePoint_);
    }

    TimeSpan operator-(const TimeSpan& timeSpan) const noexcept {
        return TimeSpan(timePoint_.time_since_epoch() - timeSpan.getDuration());
    }

    DateTime& operator-=(const TimeSpan& timeSpan) {
        timePoint_ -= timeSpan.getDuration();
        return *this;
    }

    TimeSpan operator-(int32 seconds) const noexcept {
        return TimeSpan(timePoint_.time_since_epoch() - Seconds{ seconds });
    }

    DateTime& operator-=(int32 seconds) {
        timePoint_ -= Seconds{ seconds };
        return *this;
    }

    ////// Comparison operators ------------------------

    // Equality operator
    constexpr bool operator==(const DateTime& other) const noexcept {
        return timePoint_ == other.timePoint_;
    }

    // Inequality operator
    constexpr bool operator!=(const DateTime& other) const noexcept {
        return timePoint_ != other.timePoint_;
    }

    // Less than operator
    constexpr bool operator<(const DateTime& other) const noexcept {
        return timePoint_ < other.timePoint_;
    }

    // Greater than operator
    constexpr bool operator>(const DateTime& other) const noexcept {
        return timePoint_ > other.timePoint_;
    }

    // Less than or equal to operator
    constexpr bool operator<=(const DateTime& other) const noexcept {
        return timePoint_ <= other.timePoint_;
    }

    // Greater than or equal to operator
    constexpr bool operator>=(const DateTime& other) const noexcept {
        return timePoint_ >= other.timePoint_;
    }

    ////// Increment and decrement operators ------------------------

    // Prefix increment operator (increase by 1 second)
    DateTime& operator++() noexcept {
        timePoint_ += Seconds{ 1 };
        return *this;
    }

    // Prefix decrement operator (decrease by 1 second)
    DateTime& operator--() noexcept {
        timePoint_ -= Seconds{ 1 };
        return *this;
    }

    // Postfix increment operator (increase by 1 second)
    DateTime operator++(int32) noexcept {
        DateTime original = *this;
        timePoint_ += Seconds{ 1 };
        return original;
    }

    // Postfix decrement operator (decrease by 1 second)
    DateTime operator--(int32) noexcept {
        DateTime original = *this;
        timePoint_ -= Seconds{ 1 };
        return original;
    }

private:
    ////// Internal getters ------------------------

    // Get the date portion in the internal calendar representation.
    constexpr CalendarDate dateVal() const noexcept {
        return CalendarDate{ NEX_STD chrono::floor<Days>(timePoint_) };
    }

    // Get the month value (1-12)
    constexpr Month monthVal() const noexcept {
        return CalendarDate{ NEX_STD chrono::floor<Days>(timePoint_) }.month();
    }

    // Get the day value (1-31)
    constexpr Day dayVal() const noexcept {
        return CalendarDate{ NEX_STD chrono::floor<Days>(timePoint_) }.day();
    }

    // Get the year value
    constexpr Year yearVal() const noexcept {
        return CalendarDate{ NEX_STD chrono::floor<Days>(timePoint_) }.year();
    }

    // Get the weekday value (0-6, Sunday=0)
    constexpr Weekday weekdayVal() const noexcept {
        return Weekday{ NEX_STD chrono::floor<Days>(timePoint_) };
    }

    // Get the time portion as a ClockTime object
    constexpr Hours getHours() const noexcept {
        return NEX_STD chrono::duration_cast<Hours>(
                                    timePoint_.time_since_epoch() % Days(1));
    }

    // Get the minute portion (0-59)
    constexpr Minutes getMinutes() const noexcept {
        return NEX_STD chrono::duration_cast<Minutes>(
                                    timePoint_.time_since_epoch() % Hours(1));
    }

    // Get the second portion (0-59)
    constexpr Seconds getSeconds() const noexcept {
        return NEX_STD chrono::duration_cast<Seconds>(
                                    timePoint_.time_since_epoch() % Minutes(1));
    }

    // Get the millisecond portion (0-999)
    constexpr Milliseconds getMillisecs() const noexcept {
        return NEX_STD chrono::duration_cast<Milliseconds>(
                                    timePoint_.time_since_epoch() % Seconds(1));
    }

public:
    ////// Special validations ------------------------

    // Validate the hour value (0-23)
    static constexpr bool isValidHour(int32 hour) noexcept {
        return chrono::isValidHour(hour);
    }

    // Validate the minute value (0-59)
    static constexpr bool isValidMinute(int32 minute) noexcept {
        return chrono::isValidMinute(minute);
    }

    // Validate the second value (0-59)
    static constexpr bool isValidSecond(int32 second) noexcept {
        return chrono::isValidSecond(second);
    }

    // Validate the millisecond value (0-999)
    static constexpr bool isValidMillisecs(int32 millisecs) noexcept {
        return chrono::isValidMillisecs(millisecs);
    }

    ////// Access individual components ------------------------

    // Get the year value
    constexpr int32 year(void) const noexcept {
        return static_cast<int32>(yearVal());
    }

    // Get the month value (1-12)
    constexpr uint32 month(void) const noexcept {
        return static_cast<uint32>(monthVal());
    }

    // Get the day value (1-31)
    constexpr uint32 day(void) const noexcept {
        return static_cast<uint32>(dayVal());
    }

    // Get the hour value (0-23)
    constexpr int32 hour(void) const noexcept {
        return getHours().count();
    }

    // Get the minute value (0-59)
    constexpr int32 minute(void) const noexcept {
        return getMinutes().count();
    }

    // Get the second value (0-59)
    constexpr int32 second(void) const noexcept {
        return static_cast<int32>(getSeconds().count());
    }

    // Get the millisecond value (0-999)
    constexpr int32 millisecond(void) const noexcept {
        return static_cast<int32>(getMillisecs().count());
    }

    // Get the day of week value (0-6, Sunday=0)
    constexpr uint8 dayOfWeek(void) const noexcept {
        return static_cast<uint8>(weekdayVal().c_encoding());
    }

    ////// Modify individual components ------------------------

    // Set the entire date (year, month, day)
    void setDate(int32 year, uint32 month, uint32 day) noexcept {
        NEX_ASSERT(isValidDate(year, month, day));
        DateTime original = *this;
        auto newDateVal = CalendarDate{ Year(year), Month(month), Day(day) };
        timePoint_ = SysDays{ newDateVal } + (original.timePoint_ - SysDays{ original.dateVal() });
    }

    // Set the entire time (hour, minute, second, millisecond)
    void setClockTime(int32 hour, int32 minute, int32 second, int32 millisecs = 0) noexcept {
        NEX_ASSERT(isValidTime(hour, minute, second, millisecs));
        DateTime original = *this;
        auto newTimeVal = Hours(hour) + Minutes(minute) + Seconds(second) + Milliseconds(millisecs);
        timePoint_ = SysDays{ original.dateVal() } + newTimeVal;
    }

    // Set the time from ClockTime value
    void setClockTime(const ClockTime& clockTime) noexcept {
        DateTime original = *this;
        auto newTimeVal = clockTime.getDuration();
        timePoint_ = SysDays{ original.dateVal() } + newTimeVal;
    }

    // Set the year value
    void setYear(int32 year) noexcept {
        DateTime original = *this;
        NEX_ASSERT(isValidDate(year, 
                               static_cast<uint32>(original.monthVal()), 
                               static_cast<uint32>(original.dayVal())));
        auto newDateVal = CalendarDate{ Year(year), original.monthVal(), original.dayVal() };
        timePoint_ = SysDays{ newDateVal } 
                        + (original.timePoint_ - SysDays{ original.dateVal() });
    }

    // Set the month value
    void setMonth(int32 month) noexcept {
        DateTime original = *this;
        NEX_ASSERT(isValidDate(static_cast<int32>(original.yearVal()), 
                               static_cast<uint32>(month), 
                               static_cast<uint32>(original.dayVal())));
        auto newDateVal = CalendarDate{ original.yearVal(), Month(month), original.dayVal() };
        timePoint_ = SysDays{ newDateVal } + (original.timePoint_ - SysDays{ original.dateVal() });
    }

    // Set the day value
    void setDay(int32 day) noexcept {
        DateTime original = *this;
        NEX_ASSERT(isValidDate(static_cast<int32>(original.yearVal()), 
                               static_cast<uint32>(original.monthVal()), 
                               static_cast<uint32>(day)));
        auto newDateVal = CalendarDate{ original.yearVal(), original.monthVal(), Day(day) };
        timePoint_ = SysDays{ newDateVal } + (original.timePoint_ - SysDays{ original.dateVal() });

    }
    // Set the hour value
    void setHour(int32 hour) noexcept {
        NEX_ASSERT(isValidHour(hour));
        DateTime original = *this;
        auto newTimeVal = Hours(hour) + original.getMinutes() 
                            + original.getSeconds() + original.getMillisecs();
        timePoint_ = SysDays{ original.dateVal() } + newTimeVal;
    }

    // Set the minute value
    void setMinute(int32 minute) noexcept {
        NEX_ASSERT(isValidMinute(minute));
        DateTime original = *this;
        auto newTimeVal = original.getHours() + Minutes(minute) 
                            + original.getSeconds() + original.getMillisecs();
        timePoint_ = SysDays{ original.dateVal() } + newTimeVal;
    }

    // Set the second value
    void setSecond(int32 second) noexcept {
        NEX_ASSERT(isValidSecond(second));
        DateTime original = *this;
        auto newTimeVal = original.getHours() + original.getMinutes() 
                            + Seconds(second) + original.getMillisecs();
        timePoint_ = SysDays{ original.dateVal() } + newTimeVal;
    }

    // Set the millisecond value
    void setMillisecs(int32 millisecs) noexcept {
        NEX_ASSERT(isValidMillisecs(millisecs));
        DateTime original = *this;
        auto newTimeVal = original.getHours() + original.getMinutes() 
                            + original.getSeconds() + Milliseconds(millisecs);
        timePoint_ = SysDays{ original.dateVal() } + newTimeVal;
    }

    ////// Special queries ------------------------

    // Check if the year is a leap year
    constexpr bool isLeapYear(void) const noexcept {
        return yearVal().is_leap();
    }

    // Validate a complete date (year, month, day)
    static constexpr bool isValidDate(int32 year, uint32 month, uint32 day) noexcept {
        return CalendarDate{ Year(year), Month(month), Day(day) }.ok();
    }

    // Validate a complete time (hour, minute, second, millisecond)
    static constexpr bool isValidTime(int32 hour, int32 minute, int32 second, int32 millisecs = 0) noexcept {
        return (chrono::isValidHour(hour) && chrono::isValidMinute(minute)
                && chrono::isValidSecond(second) && chrono::isValidMillisecs(millisecs));
    }

    // Check if the DateTime represents today's date
    constexpr bool isToday(void) const noexcept {
        return dateVal() == DateTime(NEX_STD chrono::system_clock::now()).dateVal();
    }

    // Check if the DateTime is in the future
    constexpr bool isInTheFuture(void) const noexcept {
        return timePoint_ > NEX_STD chrono::system_clock::now();
    }

    // Check if the DateTime is in the past
    constexpr bool isInThePast(void) const noexcept {
        return timePoint_ < NEX_STD chrono::system_clock::now();
    }

    ////// Arithmetic methods ------------------------

    // Increase the date by a specified number of days
    DateTime& increaseDays(int32 days) noexcept {
        timePoint_ += Days{ days };
        return *this;
    }

    // Decrease the date by a specified number of days
    DateTime& decreaseDays(int32 days) noexcept {
        timePoint_ -= Days{ days };
        return *this;
    }

    // Increase the time by a specified number of hours
    DateTime& increaseHours(int32 hours) noexcept {
        timePoint_ += Hours{ hours };
        return *this;
    }

    // Decrease the time by a specified number of hours
    DateTime& decreaseHours(int32 hours) noexcept {
        timePoint_ -= Hours{ hours };
        return *this;
    }

    // Increase the time by a specified number of minutes
    DateTime& increaseMinutes(int32 mins) noexcept {
        timePoint_ += Minutes{ mins };
        return *this;
    }

    // Decrease the time by a specified number of minutes
    DateTime& decreaseMinutes(int32 mins) noexcept {
        timePoint_ -= Minutes{ mins };
        return *this;
    }

    // Increase the time by a specified number of seconds
    DateTime& increaseSeconds(int32 secs) noexcept {
        timePoint_ += Seconds{ secs };
        return *this;
    }

    // Decrease the time by a specified number of seconds
    DateTime& decreaseSeconds(int32 secs) noexcept {
        timePoint_ -= Seconds{ secs };
        return *this;
    }

    // Increase the time by a specified number of milliseconds
    DateTime& increaseMillisecs(int32 millisecs) noexcept {
        timePoint_ += Milliseconds{ millisecs };
        return *this;
    }

    // Decrease the time by a specified number of milliseconds
    DateTime& decreaseMillisecs(int32 millisecs) noexcept {
        timePoint_ -= Milliseconds{ millisecs };
        return *this;
    }

    ////// String conversion ------------------------
    
    /**
     * @brief  Convert DateTime to string representation
     * @param  format  Format string. Supported formats:
     *                 - "default" or empty: "YYYY-MM-DD HH:MM:SS"
     *                 - "with_ms": "YYYY-MM-DD HH:MM:SS.mmm" (with milliseconds)
     *                 - "iso": "YYYY-MM-DDTHH:MM:SS" (ISO 8601)
     *                 - "iso_ms": "YYYY-MM-DDTHH:MM:SS.mmm" (ISO 8601 with milliseconds)
     *                 - "us": "MM/DD/YYYY HH:MM:SS" (US date format)
     *                 - "us_12h": "MM/DD/YYYY HH:MM:SS AM/PM" (US format with 12-hour time)
     * @return Formatted string representation
     */
    String toString(wcstring format = L"default") const;

    /**
     * @brief  Create DateTime from string representation
     * @param  str     String to parse. Supports formats:
     *                 - "YYYY-MM-DD HH:MM:SS" or "YYYY-MM-DD HH:MM:SS.mmm"
     *                 - "YYYY-MM-DDTHH:MM:SS" or "YYYY-MM-DDTHH:MM:SS.mmm" (ISO 8601)
     *                 - "MM/DD/YYYY HH:MM:SS" or "MM/DD/YYYY HH:MM:SS.mmm" (US format)
     *                 - "MM/DD/YYYY HH:MM:SS AM/PM" (US format with 12-hour time)
     * @return Parsed DateTime, or empty DateTime on failure
     */
    static DateTime fromString(const String& str);
};

NEX_LAYER_NAMESPACE_END(chrono)
