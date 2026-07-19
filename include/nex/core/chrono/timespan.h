/**
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (c) 2026 Anthony Lee Stark. All rights reserved.
 */

#pragma once

#include "nex/base/macros.h"
#include "nex/core/text/string.h"
#include "nex/core/chrono/chrono_base.h"

NEX_LAYER_NAMESPACE_BEGIN(chrono)

/**
 * @class   TimeSpan
 * @brief   Represents a time duration or span
 * 
 * TimeSpan represents an interval or duration of time, independent of any specific
 * date or time point. It is designed for measuring elapsed time, calculating time
 * differences, and performing time-based arithmetic operations.
 * 
 * @details
 * TimeSpan provides comprehensive functionality for working with time durations:
 * 
 * - Construction: Can be created from individual time components (days, hours, minutes, seconds, 
 *   and milliseconds), or from a std::chrono::duration directly.
 * - String parsing: Can be constructed from a string representation of a time span, supporting
 *   various formats (e.g., "D days, H hours, M minutes, S seconds", "HH:MM:SS", "PTnHnMnS" ISO 8601 duration).
 * - String formatting: Can be converted to a string representation in different formats.
 * - Arithmetic operations: Supports addition, subtraction, and comparison with other TimeSpan instances 
 *   or integer seconds.
 * - Comparison operators: Provides equality, inequality, and relational operators for comparing 
 *   TimeSpan instances.
 * - Increment and decrement: Supports prefix and postfix increment/decrement operators to adjust 
 *   the duration by one second.
 * - Accessors: Provides methods to retrieve the total duration in various units (days, hours, minutes, 
 *   seconds, milliseconds).
 * - Validation: Ensures that time values are valid when constructing from components or parsing from strings.
 * 
 * TimeSpan is particularly useful for:
 * - Calculating time differences between two DateTime or ClockTime instances
 * - Representing intervals for scheduling or timing operations
 * - Performing time-based calculations without date context
 * - Measuring elapsed time or timeouts
 * 
 * @note TimeSpan values can be negative, representing a backward duration
 * @note All arithmetic operations maintain precision up to milliseconds
 * @see ClockTime, DateTime
 */
class NEX_API TimeSpan {
private:
    // Internal representation of the time span as a duration
    Duration duration_{};

public:
    // Constructor
    explicit TimeSpan() = default;

    // Copy constructor and assignment
    NEX_DEFAULT_COPY(TimeSpan);

    // Move constructor and assignment
    NEX_DEFAULT_MOVE(TimeSpan);

    // Conversion constructor from Duration
    explicit TimeSpan(Duration duration) : duration_(duration) {}
    TimeSpan(int32 days, int32 hours = 0, int32 mins = 0, int32 secs = 0, int32 millisecs = 0) noexcept {
        duration_ = Days{ days } + Hours{ hours } + Minutes{ mins }
                    + Seconds{ secs } + Milliseconds{ millisecs };
    }

    ////// Accessors and utility methods ------------------------
    
    // Get the underlying duration
    Duration getDuration(void) const noexcept {
        return duration_;
    }
    // Check if the TimeSpan is empty (zero duration)
    constexpr bool isEmpty() const noexcept {
        return duration_ == Duration{};
    }

    ////// Arithmetic operators ------------------------
    
    // Addition operator
    TimeSpan operator+(const TimeSpan& other) const noexcept {
        return TimeSpan(duration_ + other.duration_);
    }
    // Compound assignment for addition
    TimeSpan& operator+=(const TimeSpan& other) {
        if (this != &other) duration_ += other.duration_;
        return *this;
    }
    // Addition with integer seconds
    TimeSpan operator+(int32 seconds) const noexcept {
        return TimeSpan(duration_ + Seconds{ seconds });
    }
    // Compound assignment for addition with integer seconds
    TimeSpan& operator+=(int32 seconds) {
        duration_ += Seconds{ seconds };
        return *this;
    }

    // Subtraction operator
    TimeSpan operator-(const TimeSpan& other) const noexcept {
        return TimeSpan(duration_ - other.duration_);
    }
    // Compound assignment for subtraction
    TimeSpan& operator-=(const TimeSpan& other) {
        if (this != &other) duration_ -= other.duration_;
        return *this;
    }
    // Subtraction with integer seconds
    TimeSpan operator-(int32 seconds) const noexcept {
        return TimeSpan(duration_ - Seconds{ seconds });
    }
    // Compound assignment for subtraction with integer seconds
    TimeSpan& operator-=(int32 seconds) {
        duration_ -= Seconds{ seconds };
        return *this;
    }

    ////// Comparison operators ------------------------

    // Equality operator
    constexpr bool operator==(const TimeSpan& other) const noexcept {
        return duration_ == other.duration_;
    }
    // Equality with integer seconds
    constexpr bool operator==(int64 seconds) const noexcept {
        return totalSeconds() == seconds;
    }

    // Inequality operator
    constexpr bool operator!=(const TimeSpan& other) const noexcept {
        return duration_ != other.duration_;
    }
    // Inequality with integer seconds
    constexpr bool operator!=(int64 seconds) const noexcept {
        return totalSeconds() != seconds;
    }

    // Comparison operator
    constexpr bool operator<(const TimeSpan& other) const noexcept {
        return duration_ < other.duration_;
    }
    // Comparison with integer seconds
    constexpr bool operator<(int64 seconds) const noexcept {
        return totalSeconds() < seconds;
    }
    // Greater than operator
    constexpr bool operator>(const TimeSpan& other) const noexcept {
        return duration_ > other.duration_;
    }
    // Greater than with integer seconds
    constexpr bool operator>(int64 seconds) const noexcept {
        return totalSeconds() > seconds;
    }
    // Less than or equal to operator
    constexpr bool operator<=(const TimeSpan& other) const noexcept {
        return duration_ <= other.duration_;
    }
    // Less than or equal to with integer seconds
    constexpr bool operator<=(int64 seconds) const noexcept {
        return totalSeconds() <= seconds;
    }
    // Greater than or equal to operator
    constexpr bool operator>=(const TimeSpan& other) const noexcept {
        return duration_ >= other.duration_;
    }
    // Greater than or equal to with integer seconds
    constexpr bool operator>=(int64 seconds) const noexcept {
        return totalSeconds() >= seconds;
    }

    ////// Increment and decrement operators ------------------------

    // Prefix increment operator (increase by 1 second)
    TimeSpan& operator++() noexcept {
        duration_ += Seconds{1};
        return *this;
    }

    // Prefix decrement operator (decrease by 1 second)
    TimeSpan& operator--() noexcept {
        duration_ -= Seconds{1};
        return *this;
    }

    // Postfix increment operator (increase by 1 second)
    TimeSpan operator++(int32) noexcept {
        TimeSpan original = *this;
        duration_ += Seconds{1};
        return original;
    }

    // Postfix decrement operator (decrease by 1 second)
    TimeSpan operator--(int32) noexcept {
        TimeSpan original = *this;
        duration_ -= Seconds{1};
        return original;
    }

    ////// Total duration accessors ------------------------
    
    // Get total hours
    constexpr int64 totalHours() const noexcept {
        return NEX_STD chrono::duration_cast<Hours>(duration_).count();
    }
    // Get total minutes
    constexpr int64 totalMinutes() const noexcept {
        return NEX_STD chrono::duration_cast<Minutes>(duration_).count();
    }
    // Get total seconds
    constexpr int64 totalSeconds() const noexcept {
        return NEX_STD chrono::duration_cast<Seconds>(duration_).count();
    }
    // Get total milliseconds
    constexpr int64 totalMilliseconds() const noexcept {
        return NEX_STD chrono::duration_cast<Milliseconds>(duration_).count();
    }

    ////// Individual component accessors ------------------------
    
    // Get the number of days
    constexpr int32 days() const noexcept {
        return static_cast<int32>(NEX_STD chrono::duration_cast<Days>(duration_).count());
    }
    // Get hours (0-23)
    constexpr int32 hours() const noexcept {
        return static_cast<int32>(totalHours() % 24);
    }
    // Get minutes (0-59)
    constexpr int32 minutes() const noexcept {
        return static_cast<int32>(totalMinutes() % 60);
    }
    // Get seconds (0-59)
    constexpr int32 seconds() const noexcept {
        return static_cast<int32>(totalSeconds() % 60);
    }
    // Get milliseconds (0-999)
    constexpr int32 milliseconds() const noexcept {
        return static_cast<int32>(totalMilliseconds() % 1000);
    }

    ////// Component modification methods ------------------------
    
    // Increase the number of days
    TimeSpan& increaseDays(int32 days) noexcept {
        duration_ += Days{ days };
        return *this;
    }
    // Decrease the number of days
    TimeSpan& decreaseDays(int32 days) noexcept {
        duration_ -= Days{ days };
        return *this;
    }
    // Increase the number of hours
    TimeSpan& increaseHours(int32 hours) noexcept {
        duration_ += Hours{ hours };
        return *this;
    }
    // Decrease the number of hours
    TimeSpan& decreaseHours(int32 hours) noexcept {
        duration_ -= Hours{ hours };
        return *this;
    }
    // Increase the number of minutes
    TimeSpan& increaseMinutes(int32 mins) noexcept {
        duration_ += Minutes{ mins };
        return *this;
    }
    // Decrease the number of minutes
    TimeSpan& decreaseMinutes(int32 mins) noexcept {
        duration_ -= Minutes{ mins };
        return *this;
    }
    // Increase the number of seconds
    TimeSpan& increaseSeconds(int32 secs) noexcept {
        duration_ += Seconds{ secs };
        return *this;
    }
    // Decrease the number of seconds
    TimeSpan& decreaseSeconds(int32 secs) noexcept {
        duration_ -= Seconds{ secs };
        return *this;
    }
    // Increase the number of milliseconds
    TimeSpan& increaseMillisecs(int32 millisecs) noexcept {
        duration_ += Milliseconds{ millisecs };
        return *this;
    }
    // Decrease the number of milliseconds
    TimeSpan& decreaseMillisecs(int32 millisecs) noexcept {
        duration_ -= Milliseconds{ millisecs };
        return *this;
    }

    ////// String conversion ------------------------

    /**
     * @brief   Convert TimeSpan to string representation
     * @param   format  Format string. Supported formats:
     *                  - "default" or empty: "D days, H hours, M minutes, S seconds"
     *                  - "compact": "HH:MM:SS" (hours can exceed 24)
     *                  - "compact_ms": "HH:MM:SS.mmm" (with milliseconds)
     *                  - "iso": "PTnHnMnS" (ISO 8601 duration format)
     * @return  Formatted string representation
     */
    String toString(wcstring format = L"default") const;

    /**
     * @brief   Create TimeSpan from string representation
     * @param   str     String to parse. Supports formats:
     *                  - "D days, H hours, M minutes, S seconds"
     *                  - "HH:MM:SS" or "HH:MM:SS.mmm"
     *                  - "PTnHnMnS" (ISO 8601 duration)
     *                  - Integer seconds (e.g., "3600")
     * @return  Parsed TimeSpan, or empty TimeSpan on failure
     */
    static TimeSpan fromString(const String& str);
};

NEX_LAYER_NAMESPACE_END(chrono)
