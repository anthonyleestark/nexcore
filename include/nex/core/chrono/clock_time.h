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
 * @class   ClockTime
 * @brief   Represents a time of day (clock time)
 * 
 * ClockTime represents a time of day without any date information, containing only
 * hours, minutes, seconds, and milliseconds. It is ideal for representing daily
 * schedules, recurring events, alarms, and any time-based operation that repeats
 * on a daily cycle.
 * 
 * @details
 * ClockTime provides comprehensive functionality for working with times of day:
 * 
 * - Construction: Can be created from individual time components (hour, minute, second, 
 *   and millisecond values) or from a std::chrono::duration representing time since midnight.
 * - String parsing: Can be constructed from a string representation of a clock time, supporting
 *   various formats (e.g., "HH:MM:SS", "HH:MM:SS.mmm", "TnHnMnS" ISO 8601 time format).
 * - String formatting: Can be converted to a string representation in different formats.
 * - Arithmetic operations: Supports addition and subtraction with TimeSpan, as well as comparison
 *   with other ClockTime instances or integer seconds since midnight.
 * - Comparison operators: Provides equality, inequality, and relational operators for comparing
 *   ClockTime instances.
 * - Time wrapping: Automatically wraps around at midnight when performing arithmetic operations, ensuring
 *   that the time remains within a 24-hour cycle.
 * - Accessors: Provides methods to retrieve the individual time components (hour, minute, second, millisecond)
 *   as well as the total time in various units (hours, minutes, seconds, milliseconds).
 * - Validation: Ensures that time values are valid when constructing from components or parsing from strings.
 * - Supports checking if the ClockTime is empty (00:00:00.000) for convenience.
 * 
 * ClockTime is particularly useful for:
 * - Daily schedules and recurring events (e.g., "every day at 3:00 PM")
 * - Alarm clocks and reminders
 * - Time-based triggers that repeat daily
 * - Representing business hours or operating schedules
 * - Time-only calculations without date context
 * 
 * @note ClockTime automatically handles time wrapping (e.g., 23:59:59 + 2 seconds = 00:00:01)
 * @note When adding/subtracting TimeSpan, the result wraps around at midnight
 * @note Comparison operations work correctly across midnight boundaries
 * @see TimeSpan, DateTime
 */
class NEX_API ClockTime {
private:
    // Internal representation of the clock time as a duration since midnight
    Duration sinceMidnight_{};

public:
    // Constructor
    explicit ClockTime() = default;

    // Copy constructor and assignment
    NEX_DEFAULT_COPY(ClockTime);

    // Move constructor and assignment
    NEX_DEFAULT_MOVE(ClockTime);

    ////// Conversion constructors -----------------------------

    // Construct from a time point (extracting time portion)
    ClockTime(TimePoint timepoint) {
        sinceMidnight_ = timepoint - NEX_STD chrono::floor<Days>(timepoint);
        wrapAround();
    }

    // Construct from a duration since midnight
    explicit ClockTime(Duration sinceMidnight) : sinceMidnight_(sinceMidnight) {
        wrapAround();
    }

    // Construct from individual time components
    ClockTime(int32 hour, int32 min, int32 sec, int32 millisec = 0) noexcept {
        NEX_ASSERT(isValidTime(hour, min, sec, millisec));
        auto _timeVal = Hours(hour) + Minutes(min) + Seconds(sec) + Milliseconds(millisec);
        sinceMidnight_ = _timeVal;
        wrapAround();
    }

    ////// Accessors and utility methods ------------------------
    
    // Get the underlying duration since midnight
    Duration getDuration(void) const noexcept {
        return sinceMidnight_;
    }
    // Get the corresponding time point (using epoch as reference)
    TimePoint getTimePoint(void) const noexcept {
        return TimePoint{ sinceMidnight_ };
    }
    // Check if the ClockTime is empty (00:00:00.000)
    constexpr bool isEmpty() const noexcept {
        return sinceMidnight_ == Duration{};
    }

private:
    ////// Internal getters ------------------------

    // Get the number of hours (0-23)
    constexpr Hours getHours() const noexcept {
        return NEX_STD chrono::duration_cast<Hours>(sinceMidnight_ % Days(1));
    }
    // Get the number of minutes (0-59)
    constexpr Minutes getMinutes() const noexcept {
        return NEX_STD chrono::duration_cast<Minutes>(sinceMidnight_ % Hours(1));
    }
    // Get the number of seconds (0-59)
    constexpr Seconds getSeconds() const noexcept {
        return NEX_STD chrono::duration_cast<Seconds>(sinceMidnight_ % Minutes(1));
    }
    // Get the number of milliseconds (0-999)
    constexpr Milliseconds getMillisecs() const noexcept {
        return NEX_STD chrono::duration_cast<Milliseconds>(sinceMidnight_ % Seconds(1));
    }
    // Convert the entire time to hours
    constexpr Hours toHours() const noexcept {
        return NEX_STD chrono::duration_cast<Hours>(sinceMidnight_);
    }
    // Convert the entire time to minutes
    constexpr Minutes toMinutes() const noexcept {
        return NEX_STD chrono::duration_cast<Minutes>(sinceMidnight_);
    }
    // Convert the entire time to seconds
    constexpr Seconds toSeconds() const noexcept {
        return NEX_STD chrono::duration_cast<Seconds>(sinceMidnight_);
    }
    // Convert the entire time to milliseconds
    constexpr Milliseconds toMillisecs() const noexcept {
        return NEX_STD chrono::duration_cast<Milliseconds>(sinceMidnight_);
    }

    // Wrap time around 24 hours (for addition/subtraction)
    void wrapAround() noexcept {
        sinceMidnight_ =
            (sinceMidnight_ % Days{1} + Days{1}) % Days{1};
    }

public:
    ////// Arithmetic operators ------------------------

    // Addition operator
    ClockTime& operator+=(const ClockTime& other) {
        if (this != &other) sinceMidnight_ += other.sinceMidnight_;
        wrapAround();
        return *this;
    }
    // Addition with TimeSpan
    TimeSpan operator+(const TimeSpan& timeSpan) const noexcept {
        return TimeSpan(sinceMidnight_ + timeSpan.getDuration());
    }
    // Compound assignment for addition with TimeSpan
    ClockTime& operator+=(const TimeSpan& timeSpan) {
        sinceMidnight_ += timeSpan.getDuration();
        wrapAround();
        return *this;
    }
    // Addition with integer seconds
    TimeSpan operator+(int32 seconds) const noexcept {
        return TimeSpan(sinceMidnight_ + Seconds{ seconds });
    }
    // Compound assignment for addition with integer seconds
    ClockTime& operator+=(int32 seconds) {
        sinceMidnight_ += Seconds{ seconds };
        wrapAround();
        return *this;
    }

    // Subtraction operator
    TimeSpan operator-(const ClockTime& other) const noexcept {
        return TimeSpan(sinceMidnight_ - other.sinceMidnight_);
    }
    // Compound assignment for subtraction
    ClockTime& operator-=(const ClockTime& other) {
        if (this != &other) sinceMidnight_ -= other.sinceMidnight_;
        wrapAround();
        return *this;
    }
    // Subtraction with TimeSpan
    TimeSpan operator-(const TimeSpan& timeSpan) const noexcept {
        return TimeSpan(sinceMidnight_ - timeSpan.getDuration());
    }
    // Compound assignment for subtraction with TimeSpan
    ClockTime& operator-=(const TimeSpan& timeSpan) {
        sinceMidnight_ -= timeSpan.getDuration();
        wrapAround();
        return *this;
    }
    // Subtraction with integer seconds
    TimeSpan operator-(int32 seconds) const noexcept {
        return TimeSpan(sinceMidnight_ - Seconds{ seconds });
    }
    // Compound assignment for subtraction with integer seconds
    ClockTime& operator-=(int32 seconds) {
        sinceMidnight_ -= Seconds{ seconds };
        wrapAround();
        return *this;
    }

    ////// Comparison operators ------------------------

    // Equality operator
    constexpr bool operator==(const ClockTime& other) const noexcept {
        return sinceMidnight_ == other.sinceMidnight_;
    }
    // Inequality operator
    constexpr bool operator!=(const ClockTime& other) const noexcept {
        return sinceMidnight_ != other.sinceMidnight_;
    }

    // Less than operator
    constexpr bool operator<(const ClockTime& other) const noexcept {
        return sinceMidnight_ < other.sinceMidnight_;
    }
    // Greater than operator
    constexpr bool operator>(const ClockTime& other) const noexcept {
        return sinceMidnight_ > other.sinceMidnight_;
    }
    // Less than or equal to operator
    constexpr bool operator<=(const ClockTime& other) const noexcept {
        return sinceMidnight_ <= other.sinceMidnight_;
    }
    // Greater than or equal to operator
    constexpr bool operator>=(const ClockTime& other) const noexcept {
        return sinceMidnight_ >= other.sinceMidnight_;
    }

    ////// Increment and decrement operators ------------------------

    // Prefix increment operator (increase by 1 second)
    ClockTime& operator++() noexcept {
        sinceMidnight_ += Seconds{ 1 };
        wrapAround();
        return *this;
    }

    // Prefix decrement operator (decrease by 1 second)
    ClockTime& operator--() noexcept {
        sinceMidnight_ -= Seconds{ 1 };
        wrapAround();
        return *this;
    }

    // Postfix increment operator (increase by 1 second)
    ClockTime operator++(int32) noexcept {
        ClockTime original = *this;
        sinceMidnight_ += Seconds{ 1 };
        wrapAround();
        return original;
    }

    // Postfix decrement operator (decrease by 1 second)
    ClockTime operator--(int32) noexcept {
        ClockTime original = *this;
        sinceMidnight_ -= Seconds{ 1 };
        wrapAround();
        return original;
    }

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
    // Validate a complete time (hour, minute, second, millisecond)
    static constexpr bool isValidTime(int32 hour, int32 minute, int32 second, int32 millisecs = 0) noexcept {
        return (chrono::isValidHour(hour) && chrono::isValidMinute(minute)
                && chrono::isValidSecond(second) && chrono::isValidMillisecs(millisecs));
    }

    ////// Access individual components ------------------------

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

    ////// Modify individual components ------------------------

    // Set the hour value (0-23)
    void setHour(int32 hour) noexcept {
        NEX_ASSERT(isValidHour(hour));
        ClockTime original = *this;
        sinceMidnight_ = Hours(hour) + original.getMinutes()
                        + original.getSeconds() + original.getMillisecs();
        wrapAround();
    }
    // Set the minute value (0-59)
    void setMinute(int32 minute) noexcept {
        NEX_ASSERT(isValidMinute(minute));
        ClockTime original = *this;
        sinceMidnight_ = original.getHours() + Minutes(minute)
                        + original.getSeconds() + original.getMillisecs();
        wrapAround();
    }
    // Set the second value (0-59)
    void setSecond(int32 second) noexcept {
        NEX_ASSERT(isValidSecond(second));
        ClockTime original = *this;
        sinceMidnight_ = original.getHours() + original.getMinutes()
                        + Seconds(second) + original.getMillisecs();
        wrapAround();
    }
    // Set the millisecond value (0-999)
    void setMillisecs(int32 millisecs) noexcept {
        NEX_ASSERT(isValidMillisecs(millisecs));
        ClockTime original = *this;
        sinceMidnight_ = original.getHours() + original.getMinutes()
                        + original.getSeconds() + Milliseconds(millisecs);
        wrapAround();
    }

    ////// Arithmetic ------------------------

    // Increase hours by a specified amount
    ClockTime& increaseHours(int32 hours) noexcept {
        sinceMidnight_ += Hours{ hours };
        wrapAround();
        return *this;
    }
    // Decrease hours by a specified amount
    ClockTime& decreaseHours(int32 hours) noexcept {
        sinceMidnight_ -= Hours{ hours };
        wrapAround();
        return *this;
    }
    // Increase minutes by a specified amount
    ClockTime& increaseMinutes(int32 mins) noexcept {
        sinceMidnight_ += Minutes{ mins };
        wrapAround();
        return *this;
    }
    // Decrease minutes by a specified amount
    ClockTime& decreaseMinutes(int32 mins) noexcept {
        sinceMidnight_ -= Minutes{ mins };
        wrapAround();
        return *this;
    }
    // Increase seconds by a specified amount
    ClockTime& increaseSeconds(int32 secs) noexcept {
        sinceMidnight_ += Seconds{ secs };
        wrapAround();
        return *this;
    }
    // Decrease seconds by a specified amount
    ClockTime& decreaseSeconds(int32 secs) noexcept {
        sinceMidnight_ -= Seconds{ secs };
        wrapAround();
        return *this;
    }
    // Increase milliseconds by a specified amount
    ClockTime& increaseMillisecs(int32 millisecs) noexcept {
        sinceMidnight_ += Milliseconds{ millisecs };
        wrapAround();
        return *this;
    }
    // Decrease milliseconds by a specified amount
    ClockTime& decreaseMillisecs(int32 millisecs) noexcept {
        sinceMidnight_ -= Milliseconds{ millisecs };
        wrapAround();
        return *this;
    }

    ////// Time comparison methods ------------------------

    // Compare this ClockTime to another and return the difference in hours
    int32 compareToHours(const ClockTime& other) const noexcept {
        return static_cast<int32>((toHours() - other.toHours()).count());
    }
    // Compare this ClockTime to another and return the difference in minutes
    int32 compareToMinutes(const ClockTime& other) const noexcept {
        return static_cast<int32>((toMinutes() - other.toMinutes()).count());
    }
    // Compare this ClockTime to another and return the difference in seconds
    int32 compareToSeconds(const ClockTime& other) const noexcept {
        return static_cast<int32>((toSeconds() - other.toSeconds()).count());
    }
    // Compare this ClockTime to another and return the difference in milliseconds
    int32 compareToMillisecs(const ClockTime& other) const noexcept {
        return static_cast<int32>((toMillisecs() - other.toMillisecs()).count());
    }

    ////// String conversion ------------------------

    /**
     * @brief   Convert ClockTime to string representation
     * @param   format  Format string. Supported formats:
     *                  - "default" or empty: "HH:MM:SS"
     *                  - "with_ms": "HH:MM:SS.mmm" (with milliseconds)
     *                  - "12h": "HH:MM:SS AM/PM" (12-hour format)
     *                  - "12h_ms": "HH:MM:SS.mmm AM/PM" (12-hour with milliseconds)
     * @return  Formatted string representation
     */
    String toString(wcstring format = L"default") const;

    /**
     * @brief   Create ClockTime from string representation
     * @param   str     String to parse. Supports formats:
     *                  - "HH:MM:SS" or "HH:MM:SS.mmm"
     *                  - "HH:MM:SS AM/PM" or "HH:MM:SS.mmm AM/PM" (12-hour format)
     *                  - "HHMM" (compact format, assumes 00 seconds)
     * @return  Parsed ClockTime, or empty ClockTime (00:00:00) on failure
     */
    static ClockTime fromString(const String& str);
};

NEX_LAYER_NAMESPACE_END(chrono)
