/**
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (c) 2026 Anthony Lee Stark. All rights reserved.
 */

#pragma once

#include <chrono>

#include "nex/base/macros.h"
#include "nex/base/types.h"
#include "nex/core/string.h"
#include "nex/base/assert_crash.h"

NEX_NAMESPACE_BEGIN

/**
 * @namespace time
 * @brief Namespace for time-related utilities and type aliases
 * 
 * The `time` namespace contains type aliases for various components of the C++20 `<chrono>` library, 
 * such as time points, durations, and calendar types. It also includes helper functions for validating 
 * time values (e.g., hours, minutes, seconds, milliseconds).
 * 
 * The type aliases provide convenient names for commonly used chrono types, making it easier 
 * to work with time-related functionality throughout the codebase. The validation functions ensure 
 * that time values are within valid ranges, which can help prevent errors when constructing 
 * time objects or performing time calculations.
 * 
 * @note The type aliases are based on the `std::chrono` library and may be used in conjunction 
 *       with other chrono types and functions.
 */
namespace time {
	////// Type aliases for chrono components ------------------------

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

	////// Helper functions for time validation ------------------------

	// Validate hour value (0-23)
	constexpr bool isValidHour(int hour) noexcept {
		return (hour >= 0 && hour <= 23);
	}
	// Validate minute value (0-59)
	constexpr bool isValidMinute(int minute) noexcept {
		return (minute >= 0 && minute <= 59);
	}
	// Validate second value (0-59)
	constexpr bool isValidSecond(int second) noexcept {
		return (second >= 0 && second <= 59);
	}
	// Validate millisecond value (0-999)
	constexpr bool isValidMillisecs(int millisecs) noexcept {
		return (millisecs >= 0 && millisecs <= 999);
	}
}

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
class NEX_EXPORT TimeSpan {
private:
	// Private type alias for the underlying duration type from the time namespace
	using Duration = time::Duration;

public:
	// Public type aliases which can be used conveniently
	using Days = time::Days;
	using Hours = time::Hours;
	using Minutes = time::Minutes;
	using Seconds = time::Seconds;
	using Milliseconds = time::Milliseconds;

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
	TimeSpan(int days, int hours = 0, int mins = 0, int secs = 0, int millisecs = 0) noexcept {
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
	TimeSpan operator+(int seconds) const noexcept {
		return TimeSpan(duration_ + Seconds{ seconds });
	}
	// Compound assignment for addition with integer seconds
	TimeSpan& operator+=(int seconds) {
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
	TimeSpan operator-(int seconds) const noexcept {
		return TimeSpan(duration_ - Seconds{ seconds });
	}
	// Compound assignment for subtraction with integer seconds
	TimeSpan& operator-=(int seconds) {
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
	TimeSpan operator++(int) noexcept {
		TimeSpan original = *this;
		duration_ += Seconds{1};
		return original;
	}

	// Postfix decrement operator (decrease by 1 second)
	TimeSpan operator--(int) noexcept {
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
	constexpr int days() const noexcept {
		return static_cast<int>(NEX_STD chrono::duration_cast<Days>(duration_).count());
	}
	// Get hours (0-23)
	constexpr int hours() const noexcept {
		return static_cast<int>(totalHours() % 24);
	}
	// Get minutes (0-59)
	constexpr int minutes() const noexcept {
		return static_cast<int>(totalMinutes() % 60);
	}
	// Get seconds (0-59)
	constexpr int seconds() const noexcept {
		return static_cast<int>(totalSeconds() % 60);
	}
	// Get milliseconds (0-999)
	constexpr int milliseconds() const noexcept {
		return static_cast<int>(totalMilliseconds() % 1000);
	}

	////// Component modification methods ------------------------
	
	// Increase the number of days
	TimeSpan& increaseDays(int days) noexcept {
		duration_ += Days{ days };
		return *this;
	}
	// Decrease the number of days
	TimeSpan& decreaseDays(int days) noexcept {
		duration_ -= Days{ days };
		return *this;
	}
	// Increase the number of hours
	TimeSpan& increaseHours(int hours) noexcept {
		duration_ += Hours{ hours };
		return *this;
	}
	// Decrease the number of hours
	TimeSpan& decreaseHours(int hours) noexcept {
		duration_ -= Hours{ hours };
		return *this;
	}
	// Increase the number of minutes
	TimeSpan& increaseMinutes(int mins) noexcept {
		duration_ += Minutes{ mins };
		return *this;
	}
	// Decrease the number of minutes
	TimeSpan& decreaseMinutes(int mins) noexcept {
		duration_ -= Minutes{ mins };
		return *this;
	}
	// Increase the number of seconds
	TimeSpan& increaseSeconds(int secs) noexcept {
		duration_ += Seconds{ secs };
		return *this;
	}
	// Decrease the number of seconds
	TimeSpan& decreaseSeconds(int secs) noexcept {
		duration_ -= Seconds{ secs };
		return *this;
	}
	// Increase the number of milliseconds
	TimeSpan& increaseMillisecs(int millisecs) noexcept {
		duration_ += Milliseconds{ millisecs };
		return *this;
	}
	// Decrease the number of milliseconds
	TimeSpan& decreaseMillisecs(int millisecs) noexcept {
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
	String toString(const wchar_t* format = L"default") const;

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
class NEX_EXPORT ClockTime {
private:
	// Private type aliases for the underlying time types from the time namespace
	using TimePoint = time::TimePoint;
	using Duration = time::Duration;
	using SysTime = time::SysTime;

public:
	// Public type aliases which can be used conveniently
	using Days = time::Days;
	using Hours = time::Hours;
	using Minutes = time::Minutes;
	using Seconds = time::Seconds;
	using Milliseconds = time::Milliseconds;

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
	ClockTime(int hour, int min, int sec, int millisec = 0) noexcept {
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
	TimeSpan operator+(int seconds) const noexcept {
		return TimeSpan(sinceMidnight_ + Seconds{ seconds });
	}
	// Compound assignment for addition with integer seconds
	ClockTime& operator+=(int seconds) {
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
	TimeSpan operator-(int seconds) const noexcept {
		return TimeSpan(sinceMidnight_ - Seconds{ seconds });
	}
	// Compound assignment for subtraction with integer seconds
	ClockTime& operator-=(int seconds) {
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
	ClockTime operator++(int) noexcept {
		ClockTime original = *this;
		sinceMidnight_ += Seconds{ 1 };
		wrapAround();
		return original;
	}

	// Postfix decrement operator (decrease by 1 second)
	ClockTime operator--(int) noexcept {
		ClockTime original = *this;
		sinceMidnight_ -= Seconds{ 1 };
		wrapAround();
		return original;
	}

	////// Special validations ------------------------

	// Validate the hour value (0-23)
	static constexpr bool isValidHour(int hour) noexcept {
		return time::isValidHour(hour);
	}
	// Validate the minute value (0-59)
	static constexpr bool isValidMinute(int minute) noexcept {
		return time::isValidMinute(minute);
	}
	// Validate the second value (0-59)
	static constexpr bool isValidSecond(int second) noexcept {
		return time::isValidSecond(second);
	}
	// Validate the millisecond value (0-999)
	static constexpr bool isValidMillisecs(int millisecs) noexcept {
		return time::isValidMillisecs(millisecs);
	}
	// Validate a complete time (hour, minute, second, millisecond)
	static constexpr bool isValidTime(int hour, int minute, int second, int millisecs = 0) noexcept {
		return (time::isValidHour(hour) && time::isValidMinute(minute)
				&& time::isValidSecond(second) && time::isValidMillisecs(millisecs));
	}

	////// Access individual components ------------------------

	// Get the hour value (0-23)
	constexpr int hour(void) const noexcept {
		return getHours().count();
	}
	// Get the minute value (0-59)
	constexpr int minute(void) const noexcept {
		return getMinutes().count();
	}
	// Get the second value (0-59)
	constexpr int second(void) const noexcept {
		return static_cast<int>(getSeconds().count());
	}
	// Get the millisecond value (0-999)
	constexpr int millisecond(void) const noexcept {
		return static_cast<int>(getMillisecs().count());
	}

	////// Modify individual components ------------------------

	// Set the hour value (0-23)
	void setHour(int hour) noexcept {
		NEX_ASSERT(isValidHour(hour));
		ClockTime original = *this;
		sinceMidnight_ = Hours(hour) + original.getMinutes()
						+ original.getSeconds() + original.getMillisecs();
		wrapAround();
	}
	// Set the minute value (0-59)
	void setMinute(int minute) noexcept {
		NEX_ASSERT(isValidMinute(minute));
		ClockTime original = *this;
		sinceMidnight_ = original.getHours() + Minutes(minute)
						+ original.getSeconds() + original.getMillisecs();
		wrapAround();
	}
	// Set the second value (0-59)
	void setSecond(int second) noexcept {
		NEX_ASSERT(isValidSecond(second));
		ClockTime original = *this;
		sinceMidnight_ = original.getHours() + original.getMinutes()
						+ Seconds(second) + original.getMillisecs();
		wrapAround();
	}
	// Set the millisecond value (0-999)
	void setMillisecs(int millisecs) noexcept {
		NEX_ASSERT(isValidMillisecs(millisecs));
		ClockTime original = *this;
		sinceMidnight_ = original.getHours() + original.getMinutes()
						+ original.getSeconds() + Milliseconds(millisecs);
		wrapAround();
	}

	////// Arithmetic ------------------------

	// Increase hours by a specified amount
	ClockTime& increaseHours(int hours) noexcept {
		sinceMidnight_ += Hours{ hours };
		wrapAround();
		return *this;
	}
	// Decrease hours by a specified amount
	ClockTime& decreaseHours(int hours) noexcept {
		sinceMidnight_ -= Hours{ hours };
		wrapAround();
		return *this;
	}
	// Increase minutes by a specified amount
	ClockTime& increaseMinutes(int mins) noexcept {
		sinceMidnight_ += Minutes{ mins };
		wrapAround();
		return *this;
	}
	// Decrease minutes by a specified amount
	ClockTime& decreaseMinutes(int mins) noexcept {
		sinceMidnight_ -= Minutes{ mins };
		wrapAround();
		return *this;
	}
	// Increase seconds by a specified amount
	ClockTime& increaseSeconds(int secs) noexcept {
		sinceMidnight_ += Seconds{ secs };
		wrapAround();
		return *this;
	}
	// Decrease seconds by a specified amount
	ClockTime& decreaseSeconds(int secs) noexcept {
		sinceMidnight_ -= Seconds{ secs };
		wrapAround();
		return *this;
	}
	// Increase milliseconds by a specified amount
	ClockTime& increaseMillisecs(int millisecs) noexcept {
		sinceMidnight_ += Milliseconds{ millisecs };
		wrapAround();
		return *this;
	}
	// Decrease milliseconds by a specified amount
	ClockTime& decreaseMillisecs(int millisecs) noexcept {
		sinceMidnight_ -= Milliseconds{ millisecs };
		wrapAround();
		return *this;
	}

	////// Time comparison methods ------------------------

	// Compare this ClockTime to another and return the difference in hours
	int compareToHours(const ClockTime& other) const noexcept {
		return static_cast<int>((toHours() - other.toHours()).count());
	}
	// Compare this ClockTime to another and return the difference in minutes
	int compareToMinutes(const ClockTime& other) const noexcept {
		return static_cast<int>((toMinutes() - other.toMinutes()).count());
	}
	// Compare this ClockTime to another and return the difference in seconds
	int compareToSeconds(const ClockTime& other) const noexcept {
		return static_cast<int>((toSeconds() - other.toSeconds()).count());
	}
	// Compare this ClockTime to another and return the difference in milliseconds
	int compareToMillisecs(const ClockTime& other) const noexcept {
		return static_cast<int>((toMillisecs() - other.toMillisecs()).count());
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
	String toString(const wchar_t* format = L"default") const;

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

/**
 * @class   DayOfWeek
 * @brief   Day of the week enumeration
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
class NEX_EXPORT DateTime {
private:
	// Private type aliases for the underlying time types from the time namespace
	using TimePoint = time::TimePoint;
	using Duration = time::Duration;
	using SysDays = time::SysDays;

public:
	// Public type aliases which can be used conveniently
	using Date = time::Date;
	using Year = time::Year;
	using Month = time::Month;
	using Day = time::Day;
	using Weekday = time::Weekday;
	using Days = time::Days;
	using Hours = time::Hours;
	using Minutes = time::Minutes;
	using Seconds = time::Seconds;
	using Milliseconds = time::Milliseconds;

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
	DateTime(int year, unsigned int month, unsigned int day, int hour, int min, int sec, int millisec = 0) noexcept {
		NEX_ASSERT(isValidDate(year, month, day));
		NEX_ASSERT(isValidTime(hour, min, sec, millisec));
		auto dateVal = Date{ Year{ year }, Month{ month }, Day{ day } };
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
	TimeSpan operator+(int seconds) const noexcept {
		return TimeSpan(timePoint_.time_since_epoch() + Seconds{ seconds });
	}
	// Compound assignment for addition with integer seconds
	DateTime& operator+=(int seconds) {
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
	TimeSpan operator-(int seconds) const noexcept {
		return TimeSpan(timePoint_.time_since_epoch() - Seconds{ seconds });
	}
	DateTime& operator-=(int seconds) {
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
	DateTime operator++(int) noexcept {
		DateTime original = *this;
		timePoint_ += Seconds{ 1 };
		return original;
	}

	// Postfix decrement operator (decrease by 1 second)
	DateTime operator--(int) noexcept {
		DateTime original = *this;
		timePoint_ -= Seconds{ 1 };
		return original;
	}

private:
	////// Internal getters ------------------------

	// Get the date portion as a Date object
	constexpr Date dateVal() const noexcept {
		return Date{ NEX_STD chrono::floor<Days>(timePoint_) };
	}
	// Get the month value (1-12)
	constexpr Month monthVal() const noexcept {
		return Date{ NEX_STD chrono::floor<Days>(timePoint_) }.month();
	}
	// Get the day value (1-31)
	constexpr Day dayVal() const noexcept {
		return Date{ NEX_STD chrono::floor<Days>(timePoint_) }.day();
	}
	// Get the year value
	constexpr Year yearVal() const noexcept {
		return Date{ NEX_STD chrono::floor<Days>(timePoint_) }.year();
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
	static constexpr bool isValidHour(int hour) noexcept {
		return time::isValidHour(hour);
	}
	// Validate the minute value (0-59)
	static constexpr bool isValidMinute(int minute) noexcept {
		return time::isValidMinute(minute);
	}
	// Validate the second value (0-59)
	static constexpr bool isValidSecond(int second) noexcept {
		return time::isValidSecond(second);
	}
	// Validate the millisecond value (0-999)
	static constexpr bool isValidMillisecs(int millisecs) noexcept {
		return time::isValidMillisecs(millisecs);
	}

	////// Access individual components ------------------------

	// Get the year value
	constexpr int year(void) const noexcept {
		return static_cast<int>(yearVal());
	}
	// Get the month value (1-12)
	constexpr unsigned month(void) const noexcept {
		return static_cast<unsigned>(monthVal());
	}
	// Get the day value (1-31)
	constexpr unsigned day(void) const noexcept {
		return static_cast<unsigned>(dayVal());
	}
	// Get the hour value (0-23)
	constexpr int hour(void) const noexcept {
		return getHours().count();
	}
	// Get the minute value (0-59)
	constexpr int minute(void) const noexcept {
		return getMinutes().count();
	}
	// Get the second value (0-59)
	constexpr int second(void) const noexcept {
		return static_cast<int>(getSeconds().count());
	}
	// Get the millisecond value (0-999)
	constexpr int millisecond(void) const noexcept {
		return static_cast<int>(getMillisecs().count());
	}
	// Get the day of week value (0-6, Sunday=0)
	constexpr int dayOfWeek(void) const noexcept {
		return static_cast<unsigned>(weekdayVal().c_encoding());
	}

	////// Modify individual components ------------------------

	// Set the entire date (year, month, day)
	void setDate(int year, unsigned int month, unsigned int day) noexcept {
		NEX_ASSERT(isValidDate(year, month, day));
		DateTime original = *this;
		auto newDateVal = Date{ Year(year), Month(month), Day(day) };
		timePoint_ = SysDays{ newDateVal } 
						+ (original.timePoint_ - SysDays{ original.dateVal() });
	}
	// Set the entire time (hour, minute, second, millisecond)
	void setClockTime(int hour, int minute, int second, int millisecs = 0) noexcept {
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
	void setYear(int year) noexcept {
		DateTime original = *this;
		NEX_ASSERT(isValidDate(year, 
									static_cast<unsigned>(original.monthVal()), 
									static_cast<unsigned>(original.dayVal())));
		auto newDateVal = Date{ Year(year), original.monthVal(), original.dayVal() };
		timePoint_ = SysDays{ newDateVal } 
						+ (original.timePoint_ - SysDays{ original.dateVal() });
	}
	// Set the month value
	void setMonth(int month) noexcept {
		DateTime original = *this;
		NEX_ASSERT(isValidDate(static_cast<int>(original.yearVal()), 
									static_cast<unsigned>(month), 
									static_cast<unsigned>(original.dayVal())));
		auto newDateVal = Date{ original.yearVal(), Month(month), original.dayVal() };
		timePoint_ = SysDays{ newDateVal } 
						+ (original.timePoint_ - SysDays{ original.dateVal() });
	}
	// Set the day value
	void setDay(int day) noexcept {
		DateTime original = *this;
		NEX_ASSERT(isValidDate(static_cast<int>(original.yearVal()), 
									static_cast<unsigned>(original.monthVal()), 
									static_cast<unsigned>(day)));
		auto newDateVal = Date{ original.yearVal(), original.monthVal(), Day(day) };
		timePoint_ = SysDays{ newDateVal } 
						+ (original.timePoint_ - SysDays{ original.dateVal() });
	}
	// Set the hour value
	void setHour(int hour) noexcept {
		NEX_ASSERT(isValidHour(hour));
		DateTime original = *this;
		auto newTimeVal = Hours(hour) + original.getMinutes() 
							+ original.getSeconds() + original.getMillisecs();
		timePoint_ = SysDays{ original.dateVal() } + newTimeVal;
	}
	// Set the minute value
	void setMinute(int minute) noexcept {
		NEX_ASSERT(isValidMinute(minute));
		DateTime original = *this;
		auto newTimeVal = original.getHours() + Minutes(minute) 
							+ original.getSeconds() + original.getMillisecs();
		timePoint_ = SysDays{ original.dateVal() } + newTimeVal;
	}
	// Set the second value
	void setSecond(int second) noexcept {
		NEX_ASSERT(isValidSecond(second));
		DateTime original = *this;
		auto newTimeVal = original.getHours() + original.getMinutes() 
							+ Seconds(second) + original.getMillisecs();
		timePoint_ = SysDays{ original.dateVal() } + newTimeVal;
	}
	// Set the millisecond value
	void setMillisecs(int millisecs) noexcept {
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
	static constexpr bool isValidDate(int year, unsigned int month, unsigned int day) noexcept {
		return Date{ Year(year), Month(month), Day(day) }.ok();
	}
	// Validate a complete time (hour, minute, second, millisecond)
	static constexpr bool isValidTime(int hour, int minute, int second, int millisecs = 0) noexcept {
		return (time::isValidHour(hour) && time::isValidMinute(minute)
				&& time::isValidSecond(second) && time::isValidMillisecs(millisecs));
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
	DateTime& increaseDays(int days) noexcept {
		timePoint_ += Days{ days };
		return *this;
	}
	// Decrease the date by a specified number of days
	DateTime& decreaseDays(int days) noexcept {
		timePoint_ -= Days{ days };
		return *this;
	}
	// Increase the time by a specified number of hours
	DateTime& increaseHours(int hours) noexcept {
		timePoint_ += Hours{ hours };
		return *this;
	}
	// Decrease the time by a specified number of hours
	DateTime& decreaseHours(int hours) noexcept {
		timePoint_ -= Hours{ hours };
		return *this;
	}
	// Increase the time by a specified number of minutes
	DateTime& increaseMinutes(int mins) noexcept {
		timePoint_ += Minutes{ mins };
		return *this;
	}
	// Decrease the time by a specified number of minutes
	DateTime& decreaseMinutes(int mins) noexcept {
		timePoint_ -= Minutes{ mins };
		return *this;
	}
	// Increase the time by a specified number of seconds
	DateTime& increaseSeconds(int secs) noexcept {
		timePoint_ += Seconds{ secs };
		return *this;
	}
	// Decrease the time by a specified number of seconds
	DateTime& decreaseSeconds(int secs) noexcept {
		timePoint_ -= Seconds{ secs };
		return *this;
	}
	// Increase the time by a specified number of milliseconds
	DateTime& increaseMillisecs(int millisecs) noexcept {
		timePoint_ += Milliseconds{ millisecs };
		return *this;
	}
	// Decrease the time by a specified number of milliseconds
	DateTime& decreaseMillisecs(int millisecs) noexcept {
		timePoint_ -= Milliseconds{ millisecs };
		return *this;
	}

	////// String conversion ------------------------
	
	/**
	 * @brief   Convert DateTime to string representation
	 * @param   format  Format string. Supported formats:
	 *                  - "default" or empty: "YYYY-MM-DD HH:MM:SS"
	 *                  - "with_ms": "YYYY-MM-DD HH:MM:SS.mmm" (with milliseconds)
	 *                  - "iso": "YYYY-MM-DDTHH:MM:SS" (ISO 8601)
	 *                  - "iso_ms": "YYYY-MM-DDTHH:MM:SS.mmm" (ISO 8601 with milliseconds)
	 *                  - "us": "MM/DD/YYYY HH:MM:SS" (US date format)
	 *                  - "us_12h": "MM/DD/YYYY HH:MM:SS AM/PM" (US format with 12-hour time)
	 * @return  Formatted string representation
	 */
	String toString(const wchar_t* format = L"default") const;

	/**
	 * @brief   Create DateTime from string representation
	 * @param   str     String to parse. Supports formats:
	 *                  - "YYYY-MM-DD HH:MM:SS" or "YYYY-MM-DD HH:MM:SS.mmm"
	 *                  - "YYYY-MM-DDTHH:MM:SS" or "YYYY-MM-DDTHH:MM:SS.mmm" (ISO 8601)
	 *                  - "MM/DD/YYYY HH:MM:SS" or "MM/DD/YYYY HH:MM:SS.mmm" (US format)
	 *                  - "MM/DD/YYYY HH:MM:SS AM/PM" (US format with 12-hour time)
	 * @return  Parsed DateTime, or empty DateTime on failure
	 */
	static DateTime fromString(const String& str);
};

NEX_NAMESPACE_END
